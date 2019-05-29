#include "tortuga/storage/tortuga_storage.h"

#include <string>

#include "folly/String.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "google/protobuf/timestamp.pb.h"
#include "google/protobuf/util/time_util.h"
#include "sqlite3.h"

#include "tortuga/time_utils.h"

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
  SqliteReset x(select_existing_task_stmt);
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
  SqliteReset x2(insert_task_stmt);

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
  SqliteReset x2(assign_task_stmt);
  assign_task_stmt->BindInt(1, retries + 1);
  assign_task_stmt->BindText(2, worked_uuid);
  assign_task_stmt->BindLong(3, CurrentTimeMillis());
  assign_task_stmt->BindLong(4, task_row_id);
  assign_task_stmt->ExecuteOrDie();
}

void TortugaStorage::CompleteTaskNotCommit(int64_t task_id, const CompleteTaskReq& req, bool done) {
  auto* complete_task_stmt = statements_->complete_task_stmt();
  SqliteReset x2(complete_task_stmt);

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

  SqliteReset x(select_task_to_complete_stmt);

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

  stmt.BindLong(++idx, task_id);

  SqliteReset x(&stmt);
  stmt.ExecuteOrDie();
}
}  // tortuga
