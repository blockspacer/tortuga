#include "tortuga/tortuga.h"

#include <chrono>
#include <string>

#include "folly/Conv.h"
#include "folly/Unit.h"
#include "glog/logging.h"
#include "grpc++/grpc++.h"

#include "tortuga/baton_handler.h"
#include "tortuga/sqlite_statement.h"
#include "tortuga/time_logger.h"

namespace tortuga {
namespace {
static const char* const kSelectExistingTaskStmt = "select rowid from tasks where id = ? and done != 1 LIMIT 1";

static const char* const kInsertTaskStmt = R"(
    insert into tasks (id, task_type, data, max_retries, priority) values (?, ?, ?, ?, ?);
)";

static const char* const kSelectWorkerUuidStmt = "select uuid from workers where worker_id = ? LIMIT 1";

static const char* const kUpdateWorkerBeatStmt = R"(
  update workers set last_beat=CURRENT_TIMESTAMP where worker_id=? ;
)";
      
static const char* const kUpdateWorkerStmt = R"(
  update workers set uuid=?, capabilities=?, last_beat=CURRENT_TIMESTAMP where worker_id=? ;
)";

static const char* const kInsertWorkerStmt = R"(
  insert into workers (uuid, worker_id, capabilities) values (?, ?, ?);
)";

static const char* kSelectTaskStmt = R"(
    select rowid, id, task_type, data, retries from tasks where worked_on != 1 and done != 1 order by priority desc limit 1;
)";

static const char* const kAssignTaskStmt = R"(
    update tasks set retries=?, worked_on=1, worker_uuid=?, started_time=CURRENT_TIMESTAMP where rowid=? ;
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
    done_time=CURRENT_TIMESTAMP,
    logs=? 
    where rowid=? ;
)";

static const char* const kSelectExpiredWorkersStmt = R"(
    select uuid, last_invalidated_uuid from workers where last_beat < datetime(?, 'unixepoch');
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

static const char* const kTaskIsDoneStmt = R"(
    select done from tasks where rowid=?;
)";
}  // anonymous namespace

TortugaHandler::TortugaHandler(sqlite3* db)
    : db_(db), 
      select_existing_task_stmt_(db, kSelectExistingTaskStmt),
      insert_task_stmt_(db_, kInsertTaskStmt),
      select_worker_uuid_stmt_(db, kSelectWorkerUuidStmt),
      update_worker_beat_stmt_(db, kUpdateWorkerBeatStmt),
      update_worker_stmt_(db, kUpdateWorkerStmt),
      insert_worker_stmt_(db, kInsertWorkerStmt),
      select_task_stmt_(db, kSelectTaskStmt),
      assign_task_stmt_(db, kAssignTaskStmt),
      select_task_to_complete_stmt_(db, kSelectTaskToCompleteStmt),
      complete_task_stmt_(db, kCompleteTaskStmt),
      select_expired_workers_stmt_(db, kSelectExpiredWorkersStmt),
      unassign_tasks_stmt_(db, kUnassignTasksStmt),
      update_worker_invalidated_uuid_stmt_(db, kUpdateWorkerInvalidatedUuidStmt),
      task_id_done_stmt_(db, kTaskIsDoneStmt) {
}

void TortugaHandler::HandleCreateTask(RpcOpts opts) {
  BatonHandler handler;

  grpc::ServerContext ctx;
  CreateReq req;
  grpc::ServerAsyncResponseWriter<CreateResp> resp(&ctx);

  // start a new RPC and wait.
  opts.tortuga_grpc->RequestCreateTask(&ctx, &req, &resp, opts.cq, opts.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  opts.fibers->addTask([this, opts]() {
    HandleCreateTask(opts);
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

void TortugaHandler::HandleRequestTask(RpcOpts opts) {
  BatonHandler handler;

  grpc::ServerContext ctx;
  TaskReq req;
  grpc::ServerAsyncResponseWriter<TaskResp> resp(&ctx);

  // start a new RPC and wait.
  opts.tortuga_grpc->RequestRequestTask(&ctx, &req, &resp, opts.cq, opts.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  opts.fibers->addTask([this, opts]() {
    HandleRequestTask(opts);
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

  insert_task_stmt_.ExecuteOrDie();
  sqlite3_int64 rowid = sqlite3_last_insert_rowid(db_);

  CreateTaskResult res;
  res.handle = folly::to<std::string>(rowid);
  res.created = true;

  return res;
}

void TortugaHandler::MaybeUpdateWorker(const Worker& worker) {
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
      update_worker_beat_stmt_.BindText(1, worker.worker_id());
      update_worker_beat_stmt_.ExecuteOrDie();
      return;
    } else {
      // The worker existed before but is outdated.
      VLOG(1) << "outdated worker: " << worker.ShortDebugString() << " versus existing uuid: " << uuid;
      UnassignTaskInExec(uuid);
  
      SqliteReset x2(&update_worker_stmt_);
      update_worker_stmt_.BindText(1, worker.uuid());
      update_worker_stmt_.BindText(2, JoinCapabilities(worker));
      update_worker_stmt_.BindText(3, worker.worker_id());

      update_worker_stmt_.ExecuteOrDie();
    }  
  } else {
    LOG(INFO) << "We are welcoming Tortuga worker to this cluster for the first time! " << worker.ShortDebugString();
    SqliteReset x2(&insert_worker_stmt_);
    insert_worker_stmt_.BindText(1, worker.uuid());
    insert_worker_stmt_.BindText(2, worker.worker_id());
    insert_worker_stmt_.BindText(3, JoinCapabilities(worker));
    insert_worker_stmt_.ExecuteOrDie();
  }
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
  SqliteReset x1(&select_task_stmt_);
  
  int rc = select_task_stmt_.Step();
  if (rc == SQLITE_DONE) {
    VLOG(3) << "Tortuga has no task at the moment";
    RequestTaskResult res;
    res.none = true;
    return res;
  }
  
  int64_t rowid = select_task_stmt_.ColumnLong(0);
  std::string id = select_task_stmt_.ColumnText(1);
  std::string task_type = select_task_stmt_.ColumnText(2);
  std::string data = select_task_stmt_.ColumnBlob(3);
  int retries = select_task_stmt_.ColumnInt(4);

  SqliteReset x2(&assign_task_stmt_);
  assign_task_stmt_.BindInt(1, retries + 1);
  assign_task_stmt_.BindText(2, worker.uuid());
  assign_task_stmt_.BindLong(3, rowid);
  assign_task_stmt_.ExecuteOrDie();

  RequestTaskResult res;
  res.none = false;
  res.id = id;
  res.handle = folly::to<std::string>(rowid);
  res.type = task_type;
  res.data = data;
  return res;
}

void TortugaHandler::HandleHeartbeat(RpcOpts opts) {
  BatonHandler handler;

  grpc::ServerContext ctx;
  Worker req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  opts.tortuga_grpc->RequestHeartbeat(&ctx, &req, &resp, opts.cq, opts.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  opts.fibers->addTask([this, opts]() {
    HandleHeartbeat(opts);
  });

  VLOG(3) << "received Heartbeat RPC: " << req.ShortDebugString();
  MaybeUpdateWorker(req);

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

void TortugaHandler::HandleCompleteTask(RpcOpts opts) {
  BatonHandler handler;

  grpc::ServerContext ctx;
  CompleteTaskReq req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  opts.tortuga_grpc->RequestCompleteTask(&ctx, &req, &resp, opts.cq, opts.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  opts.fibers->addTask([this, opts]() {
    HandleCompleteTask(opts);
  });

  VLOG(3) << "received CompleteTask RPC: " << req.ShortDebugString();
  CompleteTask(req);

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

void TortugaHandler::CompleteTask(const CompleteTaskReq& req) {
  folly::fibers::await([&](folly::fibers::Promise<folly::Unit> p) {
    exec_.add([this, &req, promise = std::move(p)]() mutable {
      CompleteTaskInExec(req);
      promise.setValue(folly::Unit());
    });
  });
}

void TortugaHandler::CompleteTaskInExec(const CompleteTaskReq& req) {
  int64_t rowid = folly::to<int64_t>(req.handle());
  VLOG(3) << "completing task of handle: " << rowid;
  SqliteReset x1(&select_task_to_complete_stmt_);

  select_task_to_complete_stmt_.BindLong(1, rowid);
  int rc = select_task_to_complete_stmt_.Step();

  if (rc == SQLITE_DONE) {
    LOG(WARNING) << "completed task doesn't exist! " << req.ShortDebugString();
    return;
  }

  std::string uuid = select_task_to_complete_stmt_.ColumnTextOrEmpty(0);

  const std::string& worker_uuid = req.worker().uuid();
  if (uuid != worker_uuid) {
    VLOG(1) << "Task doesn't belong to the worker anymore (uuid is: " << uuid << " while worker is: " << worker_uuid << ")";
    return; 
  }

  int max_retries = select_task_to_complete_stmt_.ColumnInt(1);
  int retries = select_task_to_complete_stmt_.ColumnInt(2);

  SqliteReset x2(&complete_task_stmt_);
  complete_task_stmt_.BindInt(1, req.code());
  complete_task_stmt_.BindText(2, req.error_message());

  bool ok = req.code() == grpc::StatusCode::OK;
  bool done = ok ? true : (retries >= max_retries);
  complete_task_stmt_.BindBool(3, done);

  complete_task_stmt_.BindText(4, req.logs());
  complete_task_stmt_.BindLong(5, rowid);

  complete_task_stmt_.ExecuteOrDie();
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
  auto now_seconds = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
  // a worker shall not miss a heartbeat in 30 seconds.
  auto expired_seconds = now_seconds - std::chrono::seconds(30);
  select_expired_workers_stmt_.BindLong(1, expired_seconds.count());

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
}

void TortugaHandler::HandlePing(RpcOpts opts) {
  BatonHandler handler;

  grpc::ServerContext ctx;
  google::protobuf::Empty req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  opts.tortuga_grpc->RequestPing(&ctx, &req, &resp, opts.cq, opts.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  opts.fibers->addTask([this, opts]() {
    HandlePing(opts);
  });

  VLOG(5) << "received Ping RPC: " << req.ShortDebugString();

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

void TortugaHandler::HandleQuit(RpcOpts opts) {
  BatonHandler handler;

  grpc::ServerContext ctx;
  google::protobuf::Empty req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  opts.tortuga_grpc->RequestQuitQuitQuit(&ctx, &req, &resp, opts.cq, opts.cq, &handler);
  CHECK(handler.Wait());

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
  LOG(FATAL) << "received QuitQuitQuit command, there is no coming back";
}

void TortugaHandler::HandleIsDone(RpcOpts opts) {
  BatonHandler handler;

  grpc::ServerContext ctx;
  google::protobuf::StringValue req;
  grpc::ServerAsyncResponseWriter<google::protobuf::BoolValue> resp(&ctx);

  // start a new RPC and wait.
  opts.tortuga_grpc->RequestIsDone(&ctx, &req, &resp, opts.cq, opts.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  opts.fibers->addTask([this, opts]() {
    HandleIsDone(opts);
  });

  VLOG(5) << "received IsDone RPC: " << req.ShortDebugString();

  google::protobuf::BoolValue reply;
  reply.set_value(IsDone(req.value()));
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

bool TortugaHandler::IsDone(const std::string& task_id) {
  folly::fibers::await([&](folly::fibers::Promise<bool> p) {
    exec_.add([this, &task_id, promise = std::move(p)]() mutable {
      promise.setValue(IsDoneInExec(task_id));
    });
  });
}

bool TortugaHandler::IsDoneInExec(const std::string& task_id) {
  SqliteReset x(&task_id_done_stmt_);
  task_id_done_stmt_.BindLong(1, folly::to<int64_t>(task_id));
  
  int rc = task_id_done_stmt_.Step();
  if (rc == SQLITE_DONE) {
    return false;
  }

  return task_id_done_stmt_.ColumnBool(0);
}
}  // namespace tortuga
