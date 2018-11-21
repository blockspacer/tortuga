#include "tortuga/tortuga.h"

#include <chrono>
#include <string>
#include <utility>

#include "folly/Conv.h"
#include "folly/MapUtil.h"
#include "folly/ScopeGuard.h"
#include "folly/String.h"
#include "folly/Unit.h"
#include "glog/logging.h"
#include "grpc++/grpc++.h"
#include "google/protobuf/timestamp.pb.h"
#include "google/protobuf/util/time_util.h"

#include "tortuga/baton_handler.h"
#include "tortuga/sqlite_statement.h"
#include "tortuga/time_logger.h"
#include "tortuga/time_utils.h"

using google::protobuf::Timestamp;
using google::protobuf::util::TimeUtil;

namespace tortuga {
namespace {
static const char* const kSelectExistingTaskStmt = "select rowid from tasks where id = ? and done != 1 LIMIT 1";

static const char* const kInsertTaskStmt = R"(
    insert into tasks (id, task_type, data, max_retries, priority, delayed_time, modules, created) values (?, ?, ?, ?, ?, ?, ?, ?);
)";
      
static const char* const kAssignTaskStmt = R"(
    update tasks set retries=?, worked_on=1, worker_uuid=?, started_time=? where rowid=? ;
)";

static const char* const kSelectTaskToCompleteStmt = R"(
    select worker_uuid, max_retries, retries from tasks where rowid=? ;
)";

static const char* const kCompleteTaskStmt = R"(
    update tasks set
    worked_on=0,
    progress=100.0,
    status_code=?,
    status_message=?,
    done=?,
    done_time=?,
    logs=?,
    output=?
    where rowid=? ;
)";
}  // anonymous namespace

TortugaHandler::TortugaHandler(sqlite3* db, RpcOpts rpc_opts, std::map<std::string, std::unique_ptr<Module>> modules)
    : db_(db),
      rpc_opts_(rpc_opts),
      select_existing_task_stmt_(db, kSelectExistingTaskStmt),
      insert_task_stmt_(db_, kInsertTaskStmt),
      assign_task_stmt_(db, kAssignTaskStmt),
      select_task_to_complete_stmt_(db, kSelectTaskToCompleteStmt),
      complete_task_stmt_(db, kCompleteTaskStmt),
      modules_(std::move(modules)) {
  progress_mgr_.reset(new ProgressManager(db, &exec_, rpc_opts));
  workers_manager_.reset(new WorkersManager(db, &exec_, [this](const std::string& uuid) {
    // cleanup the prepared statement that would otherwise take up memory.
    select_task_stmts_.erase(uuid);
  }));

  workers_manager_->LoadWorkers();
}

void TortugaHandler::HandleCreateTask() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  CreateReq req;
  grpc::ServerAsyncResponseWriter<CreateResp> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestCreateTask(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleCreateTask();
  });

  VLOG(3) << "received CreateTask RPC: " << req.ShortDebugString();

  CreateTaskResult res = CreateTask(req.task());
  if (res.created) {
    const auto& modules = req.task().modules();
    std::vector<std::string> mods_vec(modules.begin(), modules.end());
    MaybeNotifyModulesOfCreation(res.handle, mods_vec);
  }

  CreateResp reply;
  reply.set_handle(res.handle);
  reply.set_created(res.created);

  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

void TortugaHandler::HandleRequestTask() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  TaskReq req;
  grpc::ServerAsyncResponseWriter<TaskResp> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestRequestTask(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleRequestTask();
  });

  VLOG(3) << "received RequestTask RPC: " << req.ShortDebugString();
  TaskResp reply;

  if (!workers_manager_->IsKnownWorker(req.worker())) {
    // For simplicity reasons, if this is a new worker, we never send it tasks until it has been declared.
    // Hopefully this worker has a functioning Heartbeat() that will declare it real soon.
    reply.set_none(true);
  } else {
    RequestTaskResult res = RequestTask(req.worker(), ctx.deadline(), true);
    if (res.none) {
      reply.set_none(true);
    } else {
      reply.set_id(res.id);
      reply.set_type(res.type);
      CHECK(reply.mutable_data()->ParseFromString(res.data));
      reply.set_handle(folly::to<std::string>(res.handle));
      reply.mutable_retry_ctx()->set_retries(res.retries);
      reply.mutable_retry_ctx()->set_progress_metadata(res.progress_metadata);
      reply.set_priority(res.priority);
    }
  }

  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

TortugaHandler::CreateTaskResult TortugaHandler::CreateTask(const Task& task) {
  auto result = folly::fibers::await([&](folly::fibers::Promise<CreateTaskResult> p) {
    exec_.add([this, &task, promise = std::move(p)]() mutable {
      promise.setValue(CreateTaskInExec(task));
    });
  });

  // See if we can notify someone.
  std::set<folly::fibers::Baton*>& batons = waiting_for_tasks_[task.type()];
  if (!batons.empty()) {
    auto it = batons.begin();
    folly::fibers::Baton* baton = *it;
    batons.erase(it);
    baton->post();
  }

  return result;
}

TortugaHandler::CreateTaskResult TortugaHandler::CreateTaskInExec(const Task& task) {
  TimeLogger create_timer("insert_task");
  SqliteTx tx(db_);
  SqliteReset x1(&select_existing_task_stmt_);
  SqliteReset x2(&insert_task_stmt_);

  select_existing_task_stmt_.BindText(1, task.id());

  int stepped = select_existing_task_stmt_.Step();
  if (stepped == SQLITE_ROW) {
    int64_t rowid = select_existing_task_stmt_.ColumnLong(0);
    VLOG(3) << "CreateTask found existing rowid: " << rowid;
    CreateTaskResult res;
    res.handle = folly::to<std::string>(rowid);
    res.created = false;
    
    return res;
  }

  VLOG(3) << "CreateTask: no existing row found for id: " << task.id();
  insert_task_stmt_.BindText(1, task.id());
  insert_task_stmt_.BindText(2, task.type());

  std::string data_str;
  CHECK(task.data().SerializeToString(&data_str));
  insert_task_stmt_.BindBlob(3, data_str);
   
  int max_retries = 3;
  if (task.has_max_retries()) {
    max_retries = task.max_retries().value();
  }

  insert_task_stmt_.BindInt(4, max_retries);

  int priority = 0;
  if (task.has_priority()) {
    priority = task.priority().value();
  }

  insert_task_stmt_.BindInt(5, priority);
  if (task.has_delay()) {
    Timestamp now = TimeUtil::GetCurrentTime();
    Timestamp delayed_time = now + task.delay();

    insert_task_stmt_.BindLong(6, TimeUtil::TimestampToMilliseconds(delayed_time));
  } else {
    insert_task_stmt_.BindNull(6);
  }

  if (task.modules_size() == 0) {
    insert_task_stmt_.BindNull(7);
  } else {
    std::string modules = folly::join(",", task.modules());
    insert_task_stmt_.BindText(7, modules);
  }

  insert_task_stmt_.BindLong(8, CurrentTimeMillis());

  insert_task_stmt_.ExecuteOrDie();
  sqlite3_int64 rowid = sqlite3_last_insert_rowid(db_);

  CreateTaskResult res;
  res.handle = folly::to<std::string>(rowid);
  res.created = true;

  return res;
}

TortugaHandler::RequestTaskResult TortugaHandler::RequestTask(const Worker& worker,
                                                              std::chrono::system_clock::time_point rpc_exp,
                                                              bool first_try) {
  RequestTaskResult res = folly::fibers::await([&](folly::fibers::Promise<RequestTaskResult> p) {
    exec_.add([this, &worker, promise = std::move(p)]() mutable {
      promise.setValue(RequestTaskInExec(worker));
    });
  });

  if (!res.none) {
    workers_manager_->OnTaskAssign(res.handle, worker.worker_id(), worker.uuid());
  } else {
    if (first_try) {
      auto now = std::chrono::system_clock::now();
      auto delay = rpc_exp - now;
      if (delay > std::chrono::seconds(1)) {
        // If there is no task, then we can sleep in the hope that there can be one soon.
        // we only do that if there is a second or more left to the RPC deadline.
        VLOG(2) << "worker: " << worker.uuid() << "will be waiting in case a task shows up";
        folly::fibers::Baton baton;
        RegisterWaitingWorker(worker, &baton);
        // we leave a second to answer and end the RPC without deadlock.
        auto wait_duration = delay - std::chrono::seconds(1);
        SCOPE_EXIT {
          UnregisterWaitingWorker(worker, &baton);
        };
        if (baton.try_wait_for(wait_duration)) {
          VLOG(1) << "cool, it was worth waiting, requesting task for: " << worker.uuid();
          return RequestTask(worker, rpc_exp, false);
        }
      }
    }
  }

  return res;
}

TortugaHandler::RequestTaskResult TortugaHandler::RequestTaskInExec(const Worker& worker) {
  TimeLogger get_task_timer("get_task");
  SqliteTx tx(db_);
  SqliteStatement* select_task_stmt = GetOrCreateSelectStmtInExec(worker);

  if (select_task_stmt == nullptr) {
    VLOG(3) << "This worker: " << worker.uuid() << " has no capabilities!";
    RequestTaskResult res;
    res.none = true;
    return res;
  }

  SqliteReset x(select_task_stmt);
  Timestamp now = TimeUtil::GetCurrentTime();
  select_task_stmt->BindLong(1, TimeUtil::TimestampToMilliseconds(now));

  int rc = select_task_stmt->Step();
  if (rc == SQLITE_DONE) {
    VLOG(3) << "Tortuga has no task at the moment";
    RequestTaskResult res;
    res.none = true;
    return res;
  }
  
  int64_t rowid = select_task_stmt->ColumnLong(0);
  std::string id = select_task_stmt->ColumnText(1);
  std::string task_type = select_task_stmt->ColumnText(2);
  std::string data = select_task_stmt->ColumnBlob(3);
  int priority = select_task_stmt->ColumnInt(4);
  int retries = select_task_stmt->ColumnInt(5);
  std::string progress_metadata = select_task_stmt->ColumnTextOrEmpty(6);

  SqliteReset x2(&assign_task_stmt_);
  assign_task_stmt_.BindInt(1, retries + 1);
  assign_task_stmt_.BindText(2, worker.uuid());
  assign_task_stmt_.BindLong(3, CurrentTimeMillis());
  assign_task_stmt_.BindLong(4, rowid);
  assign_task_stmt_.ExecuteOrDie();

  RequestTaskResult res;
  res.none = false;
  res.id = id;
  res.handle = rowid;
  res.type = task_type;
  res.data = data;
  res.priority = priority;
  res.retries = retries + 1;
  std::swap(res.progress_metadata, progress_metadata);
  return res;
}

void TortugaHandler::RegisterWaitingWorker(const Worker& worker, folly::fibers::Baton* baton) {
  for (const auto& cap : worker.capabilities()) {
    waiting_for_tasks_[cap].insert(baton);
  }
}

void TortugaHandler::UnregisterWaitingWorker(const Worker& worker, folly::fibers::Baton* baton) {
  for (const auto& cap : worker.capabilities()) {
    waiting_for_tasks_[cap].erase(baton);
  }
}

SqliteStatement* TortugaHandler::GetOrCreateSelectStmtInExec(const Worker& worker) {
  std::unique_ptr<SqliteStatement>* found = folly::get_ptr(select_task_stmts_, worker.uuid());
  if (found != nullptr) {
    return found->get();
  }

  if (worker.capabilities_size() == 0) {
    return nullptr;
  }

  std::string tpl = R"(
    select rowid, id, task_type, data, priority, retries, progress_metadata from tasks where
        worked_on != 1
        and done != 1
        and task_type IN (%s)
        and ((delayed_time IS NULL) OR (delayed_time < ?))
        order by priority desc limit 1;
  )";

  std::vector<std::string> quoted_capabilities;
  for (const auto& capa : worker.capabilities()) {
    quoted_capabilities.push_back("'" + capa + "'");
  }
  
  std::string capabilities = folly::join(", ", quoted_capabilities);
  std::string stmt_str = folly::stringPrintf(tpl.c_str(), capabilities.c_str());

  SqliteStatement* stmt = new SqliteStatement(db_, stmt_str);
  select_task_stmts_[worker.uuid()] = std::unique_ptr<SqliteStatement>(stmt);
  return stmt;
}

void TortugaHandler::HandleHeartbeat() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  HeartbeatReq req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestHeartbeat(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleHeartbeat();
  });

  VLOG(3) << "received Heartbeat RPC: " << req.ShortDebugString();
  VLOG(3) << "after this req the fibers allocated is: " << rpc_opts_.fibers->fibersAllocated()
          << " pool size: " << rpc_opts_.fibers->fibersPoolSize();

  for (const auto& worker_beat : req.worker_beats()) {
    workers_manager_->Beat(worker_beat);
  }

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

void TortugaHandler::HandleCompleteTask() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  CompleteTaskReq req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestCompleteTask(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleCompleteTask();
  });

  VLOG(3) << "received CompleteTask RPC: " << req.ShortDebugString();
  std::unique_ptr<UpdatedTask> progress(CompleteTask(req));
  if (progress != nullptr) {
    MaybeNotifyModulesOfUpdate(*progress);
    UpdateProgressManagerCache(*progress);
    workers_manager_->OnTaskComplete(folly::to<int64_t>(progress->progress->handle()),
        req.worker());
  }

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

UpdatedTask* TortugaHandler::CompleteTask(const CompleteTaskReq& req) {
  return folly::fibers::await([&](folly::fibers::Promise<UpdatedTask*> p) {
    exec_.add([this, &req, promise = std::move(p)]() mutable {
      promise.setValue(CompleteTaskInExec(req));
    });
  });
}

UpdatedTask* TortugaHandler::CompleteTaskInExec(const CompleteTaskReq& req) {
  int64_t rowid = folly::to<int64_t>(req.handle());
  VLOG(3) << "completing task of handle: " << rowid;
  SqliteReset x1(&select_task_to_complete_stmt_);

  select_task_to_complete_stmt_.BindLong(1, rowid);
  int rc = select_task_to_complete_stmt_.Step();

  if (rc == SQLITE_DONE) {
    LOG(WARNING) << "completed task doesn't exist! " << req.ShortDebugString();
    return nullptr;
  }

  std::string uuid = select_task_to_complete_stmt_.ColumnTextOrEmpty(0);

  const std::string& worker_uuid = req.worker().uuid();
  if (uuid != worker_uuid) {
    VLOG(1) << "Task doesn't belong to the worker anymore (uuid is: " << uuid << " while worker is: " << worker_uuid << ")";
    return nullptr;
  }

  int max_retries = select_task_to_complete_stmt_.ColumnInt(1);
  int retries = select_task_to_complete_stmt_.ColumnInt(2);

  SqliteReset x2(&complete_task_stmt_);
  complete_task_stmt_.BindInt(1, req.code());
  complete_task_stmt_.BindText(2, req.error_message());

  bool ok = req.code() == grpc::StatusCode::OK;
  bool done = ok ? true : (retries >= max_retries);
  complete_task_stmt_.BindBool(3, done);
  complete_task_stmt_.BindLong(4, CurrentTimeMillis());  // done_time
  complete_task_stmt_.BindText(5, req.logs());
  complete_task_stmt_.BindText(6, req.output());
  complete_task_stmt_.BindLong(7, rowid);

  complete_task_stmt_.ExecuteOrDie();
  
  return progress_mgr_->FindTaskByHandleInExec(rowid);
}

void TortugaHandler::CheckHeartbeatsLoop() {
  // This is how we do repeated TimedTasks in a fiber since nobody
  // will post to the Baton we'll just sleep/execute repeatedly.
  for (;;) {
    folly::fibers::Baton baton;
    CHECK(!baton.timed_wait(std::chrono::milliseconds(500)));
    workers_manager_->CheckHeartbeats();
  }
}

void TortugaHandler::HandleUpdateProgress() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  UpdateProgressReq req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestUpdateProgress(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleUpdateProgress();
  });

  VLOG(3) << "received HandleUpdateProgress RPC: " << req.ShortDebugString();
  std::unique_ptr<UpdatedTask> progress(UpdateProgress(req));
  if (progress != nullptr) {
    MaybeNotifyModulesOfUpdate(*progress);
    UpdateProgressManagerCache(*progress);
  }

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

UpdatedTask* TortugaHandler::UpdateProgress(const UpdateProgressReq& req) {
  return folly::fibers::await([&](folly::fibers::Promise<UpdatedTask*> p) {
    exec_.add([this, &req, promise = std::move(p)]() mutable {
      promise.setValue(UpdateProgressInExec(req));
    });
  });
}

UpdatedTask* TortugaHandler::UpdateProgressInExec(const UpdateProgressReq& req) {
  int64_t rowid = folly::to<int64_t>(req.handle());
  VLOG(3) << "updating task of handle: " << rowid;
  SqliteReset x1(&select_task_to_complete_stmt_);

  select_task_to_complete_stmt_.BindLong(1, rowid);
  int rc = select_task_to_complete_stmt_.Step();

  if (rc == SQLITE_DONE) {
    LOG(WARNING) << "updating task doesn't exist! " << req.ShortDebugString();
    return nullptr;
  }

  std::string uuid = select_task_to_complete_stmt_.ColumnTextOrEmpty(0);

  const std::string& worker_uuid = req.worker().uuid();
  if (uuid != worker_uuid) {
    VLOG(1) << "Task doesn't belong to the worker anymore (uuid is: " << uuid << " while worker is: " << worker_uuid << ")";
    return nullptr;
  }

  std::ostringstream query;
  query << "update tasks set ";

  std::vector<std::string> setters;
  if (req.has_progress()) {
    setters.push_back("progress=?");
  }

  if (req.has_progress_message()) {
    setters.push_back("progress_message=?");    
  }

  if (req.has_progress_metadata()) {
    setters.push_back("progress_metadata=?");    
  }

  query << folly::join(", ", setters);
  query << " where rowid=? ;";

  std::string query_str = query.str();
  SqliteStatement stmt(db_, query_str);

  int idx = 0;
  if (req.has_progress()) {
    stmt.BindFloat(++idx, req.progress().value());
  }

  if (req.has_progress_message()) {
    stmt.BindText(++idx, req.progress_message().value());
  }

  if (req.has_progress_metadata()) {
    stmt.BindText(++idx, req.progress_metadata().value());
  }

  stmt.BindLong(++idx, rowid);

  SqliteReset x2(&stmt);
  stmt.ExecuteOrDie();

  return progress_mgr_->FindTaskByHandleInExec(rowid);
}

void TortugaHandler::MaybeNotifyModulesOfUpdate(const UpdatedTask& task) {
  for (const auto& module_name : task.modules) {
    const std::unique_ptr<Module>* module = folly::get_ptr(modules_, module_name);
    if (module != nullptr) {
      VLOG(2) << "notifying module: " << module_name << " of task progres: " << task.progress->id();
      (*module)->OnProgressUpdate(*task.progress);
    }
  }
}

void TortugaHandler::MaybeNotifyModulesOfCreation(const std::string& handle,
                                                  const std::vector<std::string> modules) {
  TaskProgress progress;
  progress.set_handle(handle);
  *progress.mutable_created() = TimeUtil::GetCurrentTime();
  // any remaining fields as defaults is fine (worked_on = false etc...).

  for (const auto& module_name : modules) {
    const std::unique_ptr<Module>* module = folly::get_ptr(modules_, module_name);
    if (module != nullptr) {
      VLOG(2) << "notifying module: " << module_name << " of task creation: " << progress.id();
      (*module)->OnProgressUpdate(progress);
    }
  }
}

void TortugaHandler::UpdateProgressManagerCache(const UpdatedTask& task) {
  progress_mgr_->UpdateTaskProgressCache(*task.progress);
}

void TortugaHandler::HandlePing() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  google::protobuf::Empty req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestPing(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandlePing();
  });

  VLOG(5) << "received Ping RPC: " << req.ShortDebugString();

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

void TortugaHandler::HandleQuit() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  google::protobuf::Empty req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestQuitQuitQuit(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
  LOG(FATAL) << "received QuitQuitQuit command, there is no coming back";
}
}  // namespace tortuga
