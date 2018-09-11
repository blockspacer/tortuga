#include "tortuga/progress_manager.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "folly/Conv.h"
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
