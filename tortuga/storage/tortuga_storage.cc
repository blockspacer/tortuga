#include "tortuga/storage/tortuga_storage.h"

#include <string>

#include "folly/String.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "google/protobuf/timestamp.pb.h"
#include "google/protobuf/util/time_util.h"
#include "sqlite3.h"

#include "tortuga/time_utils.h"
#include "tortuga/tortuga.pb.h"
#include "tortuga/storage/fields.h"

DEFINE_string(db_file, "tortuga.db", "path to db file.");

using google::protobuf::Timestamp;
using google::protobuf::util::TimeUtil;

namespace tortuga {
namespace {
// all our times are in millis since epoch.
const char* const kCreateTortuga = R"(
  CREATE TABLE IF NOT EXISTS tasks(
    id TEXT NOT NULL,
    task_type TEXT NOT NULL,
    data BLOB NOT NULL,
    created INTEGER NOT NULL,
    max_retries INTEGER NOT NULL,
    retries INTEGER NOT NULL DEFAULT 0,
    priority INTEGER NOT NULL,
    delayed_time INTEGER NULL DEFAULT NULL,
    modules TEXT NULL DEFAULT NULL,
    worked_on BOOLEAN NOT NULL DEFAULT false,
    worker_uuid TEXT NULL DEFAULT NULL,
    progress FLOAT NOT NULL DEFAULT 0.0,
    progress_message TEXT NULL,
    progress_metadata TEXT NULL DEFAULT NULL,
    status_code INTEGER NULL DEFAULT NULL,
    status_message TEXT NULL DEFAULT NULL,
    done BOOLEAN NOT NULL DEFAULT false,
    started_time INTEGER NULL DEFAULT NULL,
    done_time INTEGER NULL DEFAULT NULL,
    logs TEXT NULL,
    output TEXT NULL DEFAULT NULL
  );

  CREATE INDEX IF NOT EXISTS tasks_id_idx ON tasks (id);

  CREATE TABLE IF NOT EXISTS workers(
    worker_id TEXT NOT NULL,
    uuid TEXT NOT NULL,
    capabilities TEXT NOT NULL DEFAULT '',
    last_beat INTEGER NOT NULL,
    last_invalidated_uuid TEXT NULL DEFAULT NULL
  );

  CREATE INDEX IF NOT EXISTS workers_worker_id_idx ON workers (worker_id);

  CREATE TABLE IF NOT EXISTS historic_workers(
    uuid TEXT NOT NULL,
    worker_id TEXT NOT NULL,
    created INTEGER NOT NULL
  );

  CREATE INDEX IF NOT EXISTS historic_workers_uuid_idx ON historic_workers (uuid);
)";

std::string JoinCapabilities(const Worker& worker) {
  return folly::join(" ", worker.capabilities());
}
}

Tx::Tx(sqlite3* db) : db_(db) {
  CHECK_EQ(SQLITE_OK, sqlite3_exec(db, "begin transaction;", nullptr, nullptr, nullptr)) << sqlite3_errmsg(db_);
}

Tx::Tx(Tx&& tx) {
  db_ = tx.db_;
  tx.db_ = nullptr;
}

Tx::~Tx() {
  if (db_ != nullptr) {
    CHECK_EQ(SQLITE_OK, sqlite3_exec(db_, "end transaction;", nullptr, nullptr, nullptr)) << sqlite3_errmsg(db_);
  }
}

TortugaStorage::TortugaStorage(sqlite3* db)
    : db_(db) {
  statements_.reset(new StatementsManager(db));
}

TortugaStorage::~TortugaStorage() {
  sqlite3_close(db_);
  db_ = nullptr;
}

std::shared_ptr<TortugaStorage> TortugaStorage::Init() {
  sqlite3* db = nullptr;
  char* err_msg = nullptr;
  int rc = sqlite3_open(FLAGS_db_file.c_str(), &db);
  CHECK_EQ(SQLITE_OK, rc) << "cound't open database "
                          << FLAGS_db_file << ": " << sqlite3_errmsg(db);

  rc = sqlite3_exec(db, kCreateTortuga, nullptr, nullptr, &err_msg);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    LOG(FATAL) << "couldn't create tortuga database: " << err_msg;
  }

  // This makes sqlite inserts much faster.
  CHECK_EQ(SQLITE_OK, sqlite3_exec(db, "PRAGMA journal_mode = WAL", nullptr, nullptr, &err_msg)) << err_msg;
  CHECK_EQ(SQLITE_OK, sqlite3_exec(db, "PRAGMA synchronous = NORMAL", nullptr, nullptr, &err_msg)) << err_msg;

  return std::shared_ptr<TortugaStorage>(new TortugaStorage(db));
}

folly::Optional<int64_t> TortugaStorage::FindTaskById(const std::string& id) {
  auto* select_existing_task_stmt = statements_->select_existing_task_stmt();
  DatabaseReset x(select_existing_task_stmt);
  select_existing_task_stmt->BindText(1, id);
  int stepped = select_existing_task_stmt->Step();
  if (stepped == SQLITE_ROW) {
    int64_t rowid = select_existing_task_stmt->ColumnLong(0);
    return rowid;
  } else {
    return folly::none;
  }
}

int64_t TortugaStorage::InsertTaskNotCommit(const Task& task) {
  auto* insert_task_stmt = statements_->insert_task_stmt();
  DatabaseReset x2(insert_task_stmt);

  insert_task_stmt->BindText(1, task.id());
  insert_task_stmt->BindText(2, task.type());

  std::string data_str;
  CHECK(task.data().SerializeToString(&data_str));
  insert_task_stmt->BindBlob(3, data_str);
   
  int max_retries = 3;
  if (task.has_max_retries()) {
    max_retries = task.max_retries().value();
  }

  insert_task_stmt->BindInt(4, max_retries);

  int priority = 0;
  if (task.has_priority()) {
    priority = task.priority().value();
  }

  insert_task_stmt->BindInt(5, priority);
  if (task.has_delay()) {
    Timestamp now = TimeUtil::GetCurrentTime();
    Timestamp delayed_time = now + task.delay();

    insert_task_stmt->BindLong(6, TimeUtil::TimestampToMilliseconds(delayed_time));
  } else {
    insert_task_stmt->BindNull(6);
  }

  if (task.modules_size() == 0) {
    insert_task_stmt->BindNull(7);
  } else {
    std::string modules = folly::join(",", task.modules());
    insert_task_stmt->BindText(7, modules);
  }

  insert_task_stmt->BindLong(8, CurrentTimeMillis());

  insert_task_stmt->ExecuteOrDie();
  return sqlite3_last_insert_rowid(db_);
}

void TortugaStorage::AssignNotCommit(int retries, const std::string& worked_uuid, int64_t task_row_id) {
  auto* assign_task_stmt = statements_->assign_task_stmt();
  DatabaseReset x2(assign_task_stmt);
  assign_task_stmt->BindInt(1, retries + 1);
  assign_task_stmt->BindText(2, worked_uuid);
  assign_task_stmt->BindLong(3, CurrentTimeMillis());
  assign_task_stmt->BindLong(4, task_row_id);
  assign_task_stmt->ExecuteOrDie();
}

void TortugaStorage::CompleteTaskNotCommit(int64_t task_id, const CompleteTaskReq& req, bool done) {
  auto* complete_task_stmt = statements_->complete_task_stmt();
  DatabaseReset x2(complete_task_stmt);

  complete_task_stmt->BindInt(1, req.code());
  complete_task_stmt->BindText(2, req.error_message());
  complete_task_stmt->BindBool(3, done);
  complete_task_stmt->BindLong(4, CurrentTimeMillis());  // done_time
  complete_task_stmt->BindText(5, req.logs());
  complete_task_stmt->BindText(6, req.output());
  complete_task_stmt->BindLong(7, task_id);

  complete_task_stmt->ExecuteOrDie();
}

folly::Optional<TaskToComplete> TortugaStorage::SelectTaskToCompleteNotCommit(int64_t task_id) {
  auto* select_task_to_complete_stmt = statements_->select_task_to_complete_stmt();

  DatabaseReset x(select_task_to_complete_stmt);

  select_task_to_complete_stmt->BindLong(1, task_id);
  int rc = select_task_to_complete_stmt->Step();

  if (rc == SQLITE_DONE) {
    return folly::none;
  }

  TaskToComplete res;
  res.worker_uuid = select_task_to_complete_stmt->ColumnTextOrEmpty(0);
  res.max_retries = select_task_to_complete_stmt->ColumnInt(1);
  res.retries = select_task_to_complete_stmt->ColumnInt(2);
  return res;
}

void TortugaStorage::UpdateProgressNotCommit(int64_t task_id, const UpdateProgressReq& req) {
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
  DatabaseStatement stmt(db_, query_str);

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

  stmt.BindLong(++idx, task_id);

  DatabaseReset x(&stmt);
  stmt.ExecuteOrDie();
}

RequestTaskResult TortugaStorage::RequestTaskNotCommit(const Worker& worker) {
  DatabaseStatement* select_task_stmt = statements_->GetOrCreateSelectStmtInExec(worker);

  if (select_task_stmt == nullptr) {
    VLOG(3) << "This worker: " << worker.uuid() << " has no capabilities!";
    RequestTaskResult res;
    res.none = true;
    return res;
  }

  DatabaseReset x(select_task_stmt);
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
  std::string modules = select_task_stmt->ColumnTextOrEmpty(7);

  RequestTaskResult res;
  res.none = false;
  res.id = id;
  res.handle = rowid;
  res.type = task_type;
  res.data = data;
  res.priority = priority;
  res.retries = retries + 1;
  std::swap(res.progress_metadata, progress_metadata);
  folly::split(",", modules, res.modules);

  return res;
}

UpdatedTask* TortugaStorage::FindUpdatedTaskByHandle(int64_t handle) {
  auto* select_task_stmt = statements_->select_task_stmt();
  select_task_stmt->BindLong(1, handle);

  return FindTaskByBoundStmt(select_task_stmt);
}

UpdatedTask* TortugaStorage::FindUpdatedTask(const TaskIdentifier& t_id) {
  auto* select_task_by_identifier_stmt = statements_->select_task_by_identifier_stmt();

  select_task_by_identifier_stmt->BindText(1, t_id.id());
  select_task_by_identifier_stmt->BindText(2, t_id.type());

  return FindTaskByBoundStmt(select_task_by_identifier_stmt);
}

UpdatedTask* TortugaStorage::FindTaskByBoundStmt(DatabaseStatement* stmt) {
  DatabaseReset x(stmt);

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

  std::string modules = stmt->ColumnTextOrEmpty(9);
  res.set_worked_on(stmt->ColumnBool(10));

  std::string worker_uuid = stmt->ColumnTextOrEmpty(11);

  res.set_progress(stmt->ColumnFloat(12));
  res.set_progress_message(stmt->ColumnTextOrEmpty(13));
  res.set_progress_metadata(stmt->ColumnTextOrEmpty(14));

  res.mutable_status()->set_code(stmt->ColumnInt(15));
  res.mutable_status()->set_message(stmt->ColumnTextOrEmpty(16));

  res.set_done(stmt->ColumnBool(17));

  auto started_time_opt = stmt->ColumnTimestamp(18);
  if (started_time_opt != nullptr) {
    *res.mutable_started_time() = *started_time_opt;
  }

  auto done_time_opt = stmt->ColumnTimestamp(19);
  if (done_time_opt != nullptr) {
    *res.mutable_done_time() = *done_time_opt;
  }

  res.set_logs(stmt->ColumnTextOrEmpty(20));
  res.set_output(stmt->ColumnTextOrEmpty(21));

  if (!worker_uuid.empty()) {
    auto* select_worker_id_by_uuid_stmt = statements_->select_worker_id_by_uuid_stmt();
    DatabaseReset x2(select_worker_id_by_uuid_stmt);
    select_worker_id_by_uuid_stmt->BindText(1, worker_uuid);
    
    // This shall alwawys be true because if a task has a worker uuid then that worker must be in the historic table.
    if (SQLITE_ROW == select_worker_id_by_uuid_stmt->Step()) {
      res.set_worker_id(select_worker_id_by_uuid_stmt->ColumnText(0));
    }
  }

  UpdatedTask* updated_task = new UpdatedTask();
  updated_task->progress = std::make_unique<TaskProgress>(res);
  folly::split(",", modules, updated_task->modules);

  return updated_task;
}

void TortugaStorage::UpdateNewWorkerNotCommit(const Worker& worker) {
  auto* update_worker_stmt = statements_->update_worker_stmt();

  DatabaseReset x(update_worker_stmt);
  update_worker_stmt->BindText(1, worker.uuid());
  update_worker_stmt->BindText(2, JoinCapabilities(worker));
  update_worker_stmt->BindLong(3, CurrentTimeMillis());
  update_worker_stmt->BindText(4, worker.worker_id());

  update_worker_stmt->ExecuteOrDie();
}

void TortugaStorage::InsertWorkerNotCommit(const Worker& worker) {
  auto* insert_worker_stmt = statements_->insert_worker_stmt();
  
  DatabaseReset x(insert_worker_stmt);
  
  insert_worker_stmt->BindText(1, worker.uuid());
  insert_worker_stmt->BindText(2, worker.worker_id());
  insert_worker_stmt->BindText(3, JoinCapabilities(worker));
  insert_worker_stmt->BindLong(4, CurrentTimeMillis());
  insert_worker_stmt->ExecuteOrDie();
}

void TortugaStorage::InsertHistoricWorkerNotCommit(const std::string& uuid,
                                                   const std::string& worker_id) {
  auto* insert_historic_worker_stmt = statements_->insert_historic_worker_stmt();
                                                  
  DatabaseReset x(insert_historic_worker_stmt);
  insert_historic_worker_stmt->BindText(1, uuid);
  insert_historic_worker_stmt->BindText(2, worker_id);
  insert_historic_worker_stmt->BindLong(3, CurrentTimeMillis());

  insert_historic_worker_stmt->ExecuteOrDie();
}

void TortugaStorage::UnassignTasksOfWorkerNotCommit(const std::string& uuid) {
  auto* unassign_tasks_stmt = statements_->unassign_tasks_stmt();

  DatabaseReset x(unassign_tasks_stmt);
  unassign_tasks_stmt->BindText(1, uuid);
  unassign_tasks_stmt->ExecuteOrDie();
}

void TortugaStorage::InvalidateExpiredWorkerNotCommit(const std::string& uuid) {
  auto* update_worker_invalidated_uuid_stmt = statements_->update_worker_invalidated_uuid_stmt();

  DatabaseReset x(update_worker_invalidated_uuid_stmt);
  update_worker_invalidated_uuid_stmt->BindText(1, uuid);
  update_worker_invalidated_uuid_stmt->BindText(2, uuid);
  update_worker_invalidated_uuid_stmt->ExecuteOrDie();
}

void TortugaStorage::UnassignTaskNotCommit(int64_t handle) {
  auto* unassign_single_task_stmt = statements_->unassign_single_task_stmt();

  DatabaseReset x(unassign_single_task_stmt);
  unassign_single_task_stmt->BindLong(1, handle);
  unassign_single_task_stmt->ExecuteOrDie();
}

namespace {
static const char* const kSelectWorkerByUuidStmt = R"(
  select * from workers where uuid = ?;
)" ;
}

folly::Optional<std::string> TortugaStorage::FindWorkerIdByUuidUnprepared(const std::string& uuid) {
  DatabaseStatement select_task_worker(db_, kSelectWorkerByUuidStmt);
  select_task_worker.BindText(1, uuid);
  int found_worker = select_task_worker.Step();
  CHECK(found_worker == SQLITE_ROW || found_worker == SQLITE_DONE);
  if (found_worker == SQLITE_DONE) {
    return folly::none;
  } else {
    return select_task_worker.ColumnText(WorkersFields::kWorkerId);
  }
}

namespace {
static const char* const kSelectTasksWorkedOnStmt = R"(
  select rowid, * from tasks where worked_on = 1 and done = 0;
)";
}

TasksWorkedOnIterator::TasksWorkedOnIterator(sqlite3* db) : db_(db), tasks_(db, kSelectTasksWorkedOnStmt) {
}

folly::Optional<TaskWorkedOn> TasksWorkedOnIterator::Next() {
  int stepped = tasks_.Step();
  CHECK(stepped == SQLITE_ROW || stepped == SQLITE_DONE) << "sql err: " << stepped
      << " err msg: " << sqlite3_errmsg(db_);
  if (stepped == SQLITE_DONE) {
    return folly::none;
  }

  TaskWorkedOn res;
  res.row_id = tasks_.ColumnLong(0);
  res.worker_uuid = tasks_.ColumnTextOrEmpty(TasksFields::kWorkerUuid + 1);  //  +1 cause we selected rowid

  return res;
}

TasksWorkedOnIterator TortugaStorage::IterateTasksWorkedOn() {
  return TasksWorkedOnIterator(db_);
}
}  // tortuga
