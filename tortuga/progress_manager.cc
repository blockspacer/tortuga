#include "tortuga/progress_manager.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "folly/Conv.h"
#include "folly/MapUtil.h"
#include "grpc++/grpc++.h"

#include "tortuga/baton_handler.h"
#include "tortuga/tortuga.grpc.pb.h"

namespace tortuga {
namespace {
static const char* const kSelectTaskStmt = R"(
    select rowid, * from tasks where rowid=? ;
)";

static const char* const kSelectTaskByIdentifierStmt = R"(
    select rowid, * from tasks where id=? and task_type=? order by created desc limit 1;
)";

static const char* const kSelectWorkerIdByUuidStmt = R"(
    select worker_id from historic_workers where uuid=? limit 1;
)";
}  // anonymous namespace

ProgressManager::ProgressManager(sqlite3* db,
                                 folly::CPUThreadPoolExecutor* exec,
                                 RpcOpts rpc_opts)
    : db_(db),
      exec_(exec),
      rpc_opts_(rpc_opts),
      select_task_stmt_(db, kSelectTaskStmt),
      select_task_by_identifier_stmt_(db, kSelectTaskByIdentifierStmt),
      select_worker_id_by_uuid_stmt_(db, kSelectWorkerIdByUuidStmt) {  
}

ProgressManager::~ProgressManager() {
}

void WriteProgressInFiber(std::shared_ptr<SubCtx> ctx,
                          RpcOpts opts,
                          std::shared_ptr<SubResp> resp) {
  opts.fibers->addTask([opts, ctx, resp]() {
    CHECK(resp != nullptr);
    CHECK(ctx != nullptr);
    BatonHandler handler;
    ctx->stream.Write(*resp, &handler);
    handler.Wait();
  });
}

void ProgressManager::HandleProgressSubscribe() {
  auto ctx = std::make_shared<SubCtx>();

  {
    BatonHandler handler;
    // start a new RPC and wait.
    rpc_opts_.tortuga_grpc->RequestProgressSubscribe(&ctx->ctx, &ctx->stream, rpc_opts_.cq, rpc_opts_.cq, &handler);
    CHECK(handler.Wait());

    VLOG(3) << "got a new progress subscription stream";

    // adds a new RPC processor.
    rpc_opts_.fibers->addTask([this]() {
      HandleProgressSubscribe();
    });
  }

  auto sub = std::make_unique<Subscription>();
  sub->ctx = ctx;

  for (;;) {
    BatonHandler handler;
    SubReq req;
    ctx->stream.Read(&req, &handler);

    if (!handler.Wait()) {
      // subscription stream is broken, they went away or they will reconnect.
      break;
    }

    VLOG(3) << "read message: " << req.ShortDebugString();
    if (req.is_beat()) {
      if (req.number_of_subs() != sub->handles.size()) {
        auto resp = std::make_shared<SubResp>();
        resp->set_must_reconnect(true);
        WriteProgressInFiber(ctx, rpc_opts_, resp);
      }

      continue;
    }

    int64_t row_id = folly::to<int64_t>(req.handle());
    std::shared_ptr<TaskProgress> progress(FindTaskByHandle(req.handle()));

    if (progress == nullptr) {
      // It shall be unreachable to ask for a task by handle but that task doesn't exist.
      BatonHandler done_handler;
      auto status_not_found = grpc::Status(grpc::StatusCode::NOT_FOUND, "no task: " + req.handle());
      ctx->stream.Finish(status_not_found, &done_handler);
      done_handler.Wait();
      break;
    }

    // If this progress is done we will write it once on the stream and we won't need to create
    // a subscription.
    if (progress->done()) {
      auto resp = std::make_shared<SubResp>();
      *resp->mutable_progress() = *progress;
      WriteProgressInFiber(ctx, rpc_opts_, resp);
    } else {
      if (sub->handles.insert(row_id).second) {
        subs_[row_id].subscriptions.push_back(sub.get());
      }  // else it was already subscribed so we don't need to.
    }
  }

  VLOG(3) << "cleaning up subscription that went away: " << sub.get();
  for (int64_t handle : sub->handles) {
    Subscriptions* subs = folly::get_ptr(subs_, handle);
    CHECK(subs != nullptr);
    std::remove(subs->subscriptions.begin(), subs->subscriptions.end(), sub.get());
  }
}

void ProgressManager::NotifyProgress(const TaskProgress& task) {
  int64_t row_id = folly::to<int64_t>(task.handle());
  Subscriptions* subs = folly::get_ptr(subs_, row_id);

  if (subs == nullptr) {
    VLOG(3) << "nobody is listening for the progress of: " << task.id();
    return;
  }

  // Make a shared copy because we will need it to live until written to all subscriber
  // streams.
  auto progress = std::make_shared<TaskProgress>(task);
  for (Subscription* sub : subs->subscriptions) {
    auto resp = std::make_shared<SubResp>();
    *resp->mutable_progress() = *progress;
    WriteProgressInFiber(sub->ctx, rpc_opts_, resp);
    
    if (progress->done()) {
      sub->handles.erase(row_id);
    }
  }
}

void ProgressManager::HandleFindTask() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  TaskIdentifier req;
  grpc::ServerAsyncResponseWriter<TaskProgress> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestFindTask(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleFindTask();
  });

  VLOG(3) << "received FindTask RPC: " << req.ShortDebugString();

  std::unique_ptr<TaskProgress> progress(FindTask(req));
  handler.Reset();
  if (progress == nullptr) {
    TaskProgress unfound;
    auto status_not_found = grpc::Status(grpc::StatusCode::NOT_FOUND, "no such task");
    resp.Finish(unfound, status_not_found, &handler);
  } else {
    resp.Finish(*progress, grpc::Status::OK, &handler);
  }

  handler.Wait();
}

void ProgressManager::HandleFindTaskByHandle() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  google::protobuf::StringValue req;
  grpc::ServerAsyncResponseWriter<TaskProgress> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestFindTaskByHandle(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleFindTaskByHandle();
  });

  VLOG(3) << "received FindTaskByHandle RPC: " << req.ShortDebugString();

  std::unique_ptr<TaskProgress> progress(FindTaskByHandle(req.value()));

  handler.Reset();
  if (progress == nullptr) {
    TaskProgress unfound;
    auto status_not_found = grpc::Status(grpc::StatusCode::NOT_FOUND, "no such task");
    resp.Finish(unfound, status_not_found, &handler);
  } else {
    resp.Finish(*progress, grpc::Status::OK, &handler);
  }

  handler.Wait();
}

TaskProgress* ProgressManager::FindTaskByHandle(const std::string& handle) {
  folly::fibers::await([&](folly::fibers::Promise<TaskProgress*> p) {
    exec_->add([this, &handle, promise = std::move(p)]() mutable {
      promise.setValue(FindTaskByHandleInExec(handle));
    });
  });
}

TaskProgress* ProgressManager::FindTaskByHandleInExec(const std::string& handle) {
  select_task_stmt_.BindLong(1, folly::to<int64_t>(handle));

  return FindTaskByBoundStmtInExec(&select_task_stmt_);
}

TaskProgress* ProgressManager::FindTask(const TaskIdentifier& t_id) {
  folly::fibers::await([&](folly::fibers::Promise<TaskProgress*> p) {
    exec_->add([this, &t_id, promise = std::move(p)]() mutable {
      promise.setValue(FindTaskInExec(t_id));
    });
  });
}

TaskProgress* ProgressManager::FindTaskInExec(const TaskIdentifier& t_id) {
  select_task_by_identifier_stmt_.BindText(1, t_id.id());
  select_task_by_identifier_stmt_.BindText(2, t_id.type());

  return FindTaskByBoundStmtInExec(&select_task_by_identifier_stmt_);
}
  
TaskProgress* ProgressManager::FindTaskByBoundStmtInExec(SqliteStatement* stmt) {
  SqliteReset x(stmt);

  int rc = stmt->Step();
  if (rc == SQLITE_DONE) {
    return nullptr;
  }

  TaskProgress res;
  int64_t handle = stmt->ColumnLong(0);
  res.set_handle(folly::to<std::string>(handle));
  res.set_id(stmt->ColumnText(1));
  res.set_type(stmt->ColumnText(2));

  auto created_opt = stmt->ColumnTimestamp(4);
  if (created_opt != nullptr) {
    *res.mutable_created() = *created_opt;
  }

  res.set_max_retries(stmt->ColumnInt(5));
  res.set_retries(stmt->ColumnInt(6));
  res.set_priority(stmt->ColumnInt(7));
  res.set_worked_on(stmt->ColumnBool(8));

  std::string worker_uuid = stmt->ColumnTextOrEmpty(9);

  res.set_progress(stmt->ColumnFloat(10));
  res.set_progress_message(stmt->ColumnTextOrEmpty(11));

  res.mutable_status()->set_code(stmt->ColumnInt(12));
  res.mutable_status()->set_message(stmt->ColumnTextOrEmpty(13));

  res.set_done(stmt->ColumnBool(14));

  auto started_time_opt = stmt->ColumnTimestamp(15);
  if (started_time_opt != nullptr) {
    *res.mutable_started_time() = *started_time_opt;
  }

  auto done_time_opt = stmt->ColumnTimestamp(16);
  if (done_time_opt != nullptr) {
    *res.mutable_done_time() = *done_time_opt;
  }

  res.set_logs(stmt->ColumnTextOrEmpty(17));

  if (!worker_uuid.empty()) {
    SqliteReset x2(&select_worker_id_by_uuid_stmt_);
    select_worker_id_by_uuid_stmt_.BindText(1, worker_uuid);
    
    // This shall alwawys be true because if a task has a worker uuid then that worker must be in the historic table.
    if (SQLITE_ROW == select_worker_id_by_uuid_stmt_.Step()) {
      res.set_worker_id(select_worker_id_by_uuid_stmt_.ColumnText(0));
    }
  }

  return new TaskProgress(res);
}
}  // namespace tortuga
