#include "tortuga/storage/tortuga_storage.h"

#include <string>

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "sqlite3.h"

DEFINE_string(db_file, "tortuga.db", "path to db file.");

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

// task statements
static const char* const kSelectExistingTaskStmt = "select rowid from tasks where id = ? and done != 1 LIMIT 1";

}

TortugaStorage::TortugaStorage(sqlite3* db)
    : db_(db),
      select_existing_task_stmt_(db, kSelectExistingTaskStmt) {

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
  SqliteReset x(&select_existing_task_stmt_);
  select_existing_task_stmt_.BindText(1, id);
  int stepped = select_existing_task_stmt_.Step();
  if (stepped == SQLITE_ROW) {
    int64_t rowid = select_existing_task_stmt_.ColumnLong(0);
    return rowid;
  } else {
    return folly::none;
  }
}

}  // tortuga
