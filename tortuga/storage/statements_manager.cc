#include "tortuga/storage/statements_manager.h"

#include "folly/MapUtil.h"
#include "folly/String.h"

#include "tortuga/tortuga.pb.h"

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

// progress manager statements
static const char* const kSelectTaskStmt = R"(
    select rowid, * from tasks where rowid=? ;
)";

static const char* const kSelectTaskByIdentifierStmt = R"(
    select rowid, * from tasks where id=? and task_type=? order by created desc limit 1;
)";

static const char* const kSelectWorkerIdByUuidStmt = R"(
    select worker_id from historic_workers where uuid=? limit 1;
)";

// worker manager statements

static const char* const kUpdateWorkerStmt = R"(
  update workers set uuid=?, capabilities=?, last_beat=? where worker_id=? ;
)";

static const char* const kInsertWorkerStmt = R"(
  insert into workers (uuid, worker_id, capabilities, last_beat) values (?, ?, ?, ?);
)";

static const char* const kInsertHistoricWorkerStmt = R"(
    insert into historic_workers(uuid, worker_id, created) values (?, ?, ?);
)";

static const char* const kUnassignTasksStmt = R"(
    update tasks set
    worked_on=0,
    worker_uuid=NULL
    where worker_uuid=? and done=0;
)";

static const char* const kUnassignSingleTaskByRowidStmt = R"(
    update tasks set
    worked_on=0,
    worker_uuid=NULL
    where rowid = ?;
)";

static const char* const kUpdateWorkerInvalidatedUuidStmt = R"(
    update workers set last_invalidated_uuid=? where uuid=?;
)";

static const char* const kSelectExpiredWorkersStmt = R"(
    select uuid, last_invalidated_uuid from workers where last_beat < ?;
)";

}  // anonymous

StatementsManager::StatementsManager(sqlite3* db)
    : db_(db),
      select_existing_task_stmt_(db, kSelectExistingTaskStmt),
      insert_task_stmt_(db, kInsertTaskStmt),
      assign_task_stmt_(db, kAssignTaskStmt),
      select_task_to_complete_stmt_(db, kSelectTaskToCompleteStmt),
      complete_task_stmt_(db, kCompleteTaskStmt),
      select_task_stmt_(db, kSelectTaskStmt),
      select_task_by_identifier_stmt_(db, kSelectTaskByIdentifierStmt),
      select_worker_id_by_uuid_stmt_(db, kSelectWorkerIdByUuidStmt),
      update_worker_stmt_(db, kUpdateWorkerStmt),
      insert_worker_stmt_(db, kInsertWorkerStmt),
      insert_historic_worker_stmt_(db, kInsertHistoricWorkerStmt),
      unassign_tasks_stmt_(db, kUnassignTasksStmt),
      unassign_single_task_stmt_(db, kUnassignSingleTaskByRowidStmt),
      update_worker_invalidated_uuid_stmt_(db, kUpdateWorkerInvalidatedUuidStmt),
      select_expired_workers_stmt_(db, kSelectExpiredWorkersStmt) {
}

StatementsManager::~StatementsManager() {}

SqliteStatement* StatementsManager::GetOrCreateSelectStmtInExec(const Worker& worker) {
  std::unique_ptr<SqliteStatement>* found = folly::get_ptr(select_task_stmts_, worker.uuid());
  if (found != nullptr) {
    return found->get();
  }

  if (worker.capabilities_size() == 0) {
    return nullptr;
  }

  std::string tpl = R"(
    select rowid, id, task_type, data, priority, retries, progress_metadata, modules from tasks where
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
}  // tortuga