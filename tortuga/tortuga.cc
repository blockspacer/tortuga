#include "tortuga/tortuga.h"

#include <chrono>
#include <string>
#include <utility>

#include "folly/Conv.h"
#include "folly/MapUtil.h"
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

static const char* const kSelectWorkerUuidStmt = "select uuid from workers where worker_id = ? LIMIT 1";

static const char* const kUpdateWorkerBeatStmt = R"(
  update workers set last_beat=? where worker_id=? ;
)";
      
static const char* const kUpdateWorkerStmt = R"(
  update workers set uuid=?, capabilities=?, last_beat=? where worker_id=? ;
)";

static const char* const kInsertWorkerStmt = R"(
  insert into workers (uuid, worker_id, capabilities, last_beat) values (?, ?, ?, ?);
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

static const char* const kSelectExpiredWorkersStmt = R"(
    select uuid, last_invalidated_uuid from workers where last_beat < ?;
)";

static const char* const kUnassignTasksStmt = R"(
    update tasks set
    worked_on=0,
    worker_uuid=NULL
    where worker_uuid=? and done=0;
)";

static const char* const kUpdateWorkerInvalidatedUuidStmt = R"(
    update workers set last_invalidated_uuid=? where uuid=?;
)";

static const char* const kInsertHistoricWorkerStmt = R"(
    insert into historic_workers(uuid, worker_id, created) values (?, ?, ?);
)";
}  // anonymous namespace

TortugaHandler::TortugaHandler(sqlite3* db, RpcOpts rpc_opts, std::map<std::string, std::unique_ptr<Module>> modules)
    : db_(db),
      rpc_opts_(rpc_opts),
      select_existing_task_stmt_(db, kSelectExistingTaskStmt),
      insert_task_stmt_(db_, kInsertTaskStmt),
      select_worker_uuid_stmt_(db, kSelectWorkerUuidStmt),
      update_worker_beat_stmt_(db, kUpdateWorkerBeatStmt),
      update_worker_stmt_(db, kUpdateWorkerStmt),
      insert_worker_stmt_(db, kInsertWorkerStmt),
      assign_task_stmt_(db, kAssignTaskStmt),
      select_task_to_complete_stmt_(db, kSelectTaskToCompleteStmt),
      complete_task_stmt_(db, kCompleteTaskStmt),
      select_expired_workers_stmt_(db, kSelectExpiredWorkersStmt),
      unassign_tasks_stmt_(db, kUnassignTasksStmt),
      update_worker_invalidated_uuid_stmt_(db, kUpdateWorkerInvalidatedUuidStmt),
      insert_historic_worker_stmt_(db, kInsertHistoricWorkerStmt),
      modules_(std::move(modules)) {
  progress_mgr_.reset(new ProgressManager(db, &exec_, rpc_opts));
  workers_manager_.reset(new WorkersManager(db, &exec_));

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
  MaybeUpdateWorker(req.worker());
  RequestTaskResult res = RequestTask(req.worker());
  TaskResp reply;
  if (res.none) {
    reply.set_none(true);
  } else {
    reply.set_id(res.id);
    reply.set_type(res.type);
    CHECK(reply.mutable_data()->ParseFromString(res.data));
    reply.set_handle(res.handle);
    reply.mutable_retry_ctx()->set_retries(res.retries);
    reply.mutable_retry_ctx()->set_progress_metadata(res.progress_metadata);
  }

  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

TortugaHandler::CreateTaskResult TortugaHandler::CreateTask(const Task& task) {
  return folly::fibers::await([&](folly::fibers::Promise<CreateTaskResult> p) {
    exec_.add([this, &task, promise = std::move(p)]() mutable {
      promise.setValue(CreateTaskInExec(task));
    });
  });
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

void TortugaHandler::MaybeUpdateWorker(const Worker& worker) {
  TimeLogger t("maybe_update_worker");
  VLOG(2) << "IOExecutor queue size is: " << exec_.getTaskQueueSize();
  folly::fibers::await([&](folly::fibers::Promise<folly::Unit> p) {
    exec_.add([this, &worker, promise = std::move(p)]() mutable {
      MaybeUpdateWorkerInExec(worker);
      promise.setValue(folly::Unit());
    });
  });
}

std::string JoinCapabilities(const Worker& worker) {
  std::ostringstream o;
  for (const std::string& cap : worker.capabilities()) {
    o << cap << " ";
  }

  return o.str();
}

void TortugaHandler::MaybeUpdateWorkerInExec(const Worker& worker) {
  SqliteReset x1(&select_worker_uuid_stmt_);
  select_worker_uuid_stmt_.BindText(1, worker.worker_id());

  int stepped = select_worker_uuid_stmt_.Step();
  if (stepped == SQLITE_ROW) {
    std::string uuid = select_worker_uuid_stmt_.ColumnText(0);
    if (worker.uuid() == uuid) {
      VLOG(3) << "worker: " << uuid << " is up to date.";

      SqliteReset x2(&update_worker_beat_stmt_);
      update_worker_beat_stmt_.BindLong(1, CurrentTimeMillis());
      update_worker_beat_stmt_.BindText(2, worker.worker_id());
      update_worker_beat_stmt_.ExecuteOrDie();
      return;
    } else {
      // The worker existed before but is outdated.
      VLOG(1) << "outdated worker: " << worker.ShortDebugString() << " versus existing uuid: " << uuid;
      UnassignTaskInExec(uuid);
  
      SqliteReset x2(&update_worker_stmt_);
      update_worker_stmt_.BindText(1, worker.uuid());
      update_worker_stmt_.BindText(2, JoinCapabilities(worker));
      update_worker_stmt_.BindLong(3, CurrentTimeMillis());
      update_worker_stmt_.BindText(4, worker.worker_id());

      update_worker_stmt_.ExecuteOrDie();
      
      InsertHistoricWorkerInExec(worker.uuid(), worker.worker_id());
    }  
  } else {
    LOG(INFO) << "We are welcoming Tortuga worker to this cluster for the first time! " << worker.ShortDebugString();
    SqliteReset x2(&insert_worker_stmt_);
    insert_worker_stmt_.BindText(1, worker.uuid());
    insert_worker_stmt_.BindText(2, worker.worker_id());
    insert_worker_stmt_.BindText(3, JoinCapabilities(worker));
    insert_worker_stmt_.BindLong(4, CurrentTimeMillis());
    insert_worker_stmt_.ExecuteOrDie();

    InsertHistoricWorkerInExec(worker.uuid(), worker.worker_id());
  }
}

void TortugaHandler::InsertHistoricWorkerInExec(const std::string& uuid,
                                                const std::string& worker_id) {
  SqliteReset x(&insert_historic_worker_stmt_);
  insert_historic_worker_stmt_.BindText(1, uuid);
  insert_historic_worker_stmt_.BindText(2, worker_id);
  insert_historic_worker_stmt_.BindLong(3, CurrentTimeMillis());

  insert_historic_worker_stmt_.ExecuteOrDie();
}

TortugaHandler::RequestTaskResult TortugaHandler::RequestTask(const Worker& worker) {
  return folly::fibers::await([&](folly::fibers::Promise<RequestTaskResult> p) {
    exec_.add([this, &worker, promise = std::move(p)]() mutable {
      promise.setValue(RequestTaskInExec(worker));
    });
  });
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
  int retries = select_task_stmt->ColumnInt(4);
  std::string progress_metadata = select_task_stmt->ColumnTextOrEmpty(5);

  SqliteReset x2(&assign_task_stmt_);
  assign_task_stmt_.BindInt(1, retries + 1);
  assign_task_stmt_.BindText(2, worker.uuid());
  assign_task_stmt_.BindLong(3, CurrentTimeMillis());
  assign_task_stmt_.BindLong(4, rowid);
  assign_task_stmt_.ExecuteOrDie();

  RequestTaskResult res;
  res.none = false;
  res.id = id;
  res.handle = folly::to<std::string>(rowid);
  res.type = task_type;
  res.data = data;
  res.retries = retries + 1;
  std::swap(res.progress_metadata, progress_metadata);
  return res;
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
    select rowid, id, task_type, data, retries, progress_metadata from tasks where
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
  Worker req;
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

  MaybeUpdateWorker(req);

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
    MaybeNotifyModules(*progress);
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
  
  return progress_mgr_->FindTaskByHandleInExec(req.handle());
}

void TortugaHandler::CheckHeartbeatsLoop() {
  // This is how we do repeated TimedTasks in a fiber since nobody
  // will post to the Baton we'll just sleep/execute repeatedly.
  for (;;) {
    folly::fibers::Baton baton;
    CHECK(!baton.timed_wait(std::chrono::milliseconds(500)));
    CheckHeartbeats();
  }
}

void TortugaHandler::CheckHeartbeats() {
  VLOG(3) << "checking heartbeats";

  folly::fibers::await([&](folly::fibers::Promise<folly::Unit> p) {
    exec_.add([this, promise = std::move(p)]() mutable {
      std::vector<std::string> uuids = ExpiredWorkersInExec();
      UnassignTasksInExec(uuids);
      promise.setValue(folly::Unit());
    });
  });

  VLOG(3) << "done checking heartbeats...";
}

std::vector<std::string> TortugaHandler::ExpiredWorkersInExec() {
  SqliteReset x1(&select_expired_workers_stmt_);
  // a worker shall not miss a heartbeat in 30 seconds.
  int64_t expired_millis = CurrentTimeMillis() - 30000L;
  select_expired_workers_stmt_.BindLong(1, expired_millis);

  std::vector<std::string> res;

  while (SQLITE_ROW == select_expired_workers_stmt_.Step()) {
    std::string uuid = select_expired_workers_stmt_.ColumnText(0);
    std::string last_invalidated_uuid = select_expired_workers_stmt_.ColumnTextOrEmpty(1);

    if (uuid != last_invalidated_uuid) {
      res.push_back(uuid);
    }
  }

  VLOG(1) << "Found: " << res.size() << " expired workers.";
  return res;
}

void TortugaHandler::UnassignTasksInExec(const std::vector<std::string>& uuids) {
  for (const auto& uuid : uuids) {
    UnassignTaskInExec(uuid);
  }
}

void TortugaHandler::UnassignTaskInExec(const std::string& uuid) {
  VLOG(2) << "unassigning tasks of expired worker: " << uuid;

  SqliteTx tx(db_);
  SqliteReset x1(&unassign_tasks_stmt_);
  unassign_tasks_stmt_.BindText(1, uuid);
  unassign_tasks_stmt_.ExecuteOrDie();

  SqliteReset x2(&update_worker_invalidated_uuid_stmt_);
  update_worker_invalidated_uuid_stmt_.BindText(1, uuid);
  update_worker_invalidated_uuid_stmt_.BindText(2, uuid);
  update_worker_invalidated_uuid_stmt_.ExecuteOrDie();

  // cleanup the prepared statement that would otherwise take up memory.
  select_task_stmts_.erase(uuid);
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
    MaybeNotifyModules(*progress);
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

  return progress_mgr_->FindTaskByHandleInExec(req.handle());
}

void TortugaHandler::MaybeNotifyModules(const UpdatedTask& task) {
  for (const auto& module_name : task.modules) {
    const std::unique_ptr<Module>* module = folly::get_ptr(modules_, module_name);
    if (module != nullptr) {
      VLOG(2) << "notifying module: " << module_name << " of task progres: " << task.progress->id();
      (*module)->OnProgressUpdate(*task.progress);
    }
  }
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
