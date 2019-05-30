#include "tortuga/storage/statements_manager.h"

#include "folly/MapUtil.h"
#include "folly/String.h"

#include "tortuga/tortuga.pb.h"

namespace tortuga {
namespace {
static const char* const kSelectExistingTaskStmt = "select id from tasks where task_id = ? and done != 1 LIMIT 1";

static const char* const kInsertTaskStmt = R"(
    insert into tasks (task_id, task_type, data, max_retries, priority, delayed_time, modules, created) values (?, ?, ?, ?, ?, FROM_UNIXTIME(?), ?, FROM_UNIXTIME(?));
)";
      
static const char* const kAssignTaskStmt = R"(
    update tasks set retries=?, worked_on=1, worker_uuid=?, started_time=FROM_UNIXTIME(?) where id=? ;
)";

static const char* const kSelectTaskToCompleteStmt = R"(
    select worker_uuid, max_retries, retries from tasks where id=? ;
)";

static const char* const kCompleteTaskStmt = R"(
    update tasks set
    worked_on=0,
    progress=100.0,
    status_code=?,
    status_message=?,
    done=?,
    done_time=FROM_UNIXTIME(?),
    logs=?,
    output=?
    where id=? ;
)";

// progress manager statements
static const char* const kSelectTaskStmt = R"(
    select * from tasks where id=? ;
)";

static const char* const kSelectTaskByIdentifierStmt = R"(
    select * from tasks where task_id=? and task_type=? order by created desc limit 1;
)";

static const char* const kSelectWorkerIdByUuidStmt = R"(
    select worker_id from historic_workers where uuid=? limit 1;
)";

// worker manager statements

static const char* const kUpdateWorkerStmt = R"(
  update workers set uuid=?, capabilities=?, last_beat=FROM_UNIXTIME(?) where worker_id=? ;
)";

static const char* const kInsertWorkerStmt = R"(
  insert into workers (uuid, worker_id, capabilities, last_beat) values (?, ?, ?, FROM_UNIXTIME(?));
)";

static const char* const kInsertHistoricWorkerStmt = R"(
    insert into historic_workers(uuid, worker_id, created) values (?, ?, FROM_UNIXTIME(?));
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
    where id = ?;
)";

static const char* const kUpdateWorkerInvalidatedUuidStmt = R"(
    update workers set last_invalidated_uuid=? where uuid=?;
)";

static const char* const kSelectExpiredWorkersStmt = R"(
    select uuid, last_invalidated_uuid from workers where last_beat < FROM_UNIXTIME(?);
)";

}  // anonymous

StatementsManager::StatementsManager(sql::Connection* conn)
    : conn_(conn),
      select_existing_task_stmt_(conn, kSelectExistingTaskStmt),
      insert_task_stmt_(conn, kInsertTaskStmt),
      assign_task_stmt_(conn, kAssignTaskStmt),
      select_task_to_complete_stmt_(conn, kSelectTaskToCompleteStmt),
      complete_task_stmt_(conn, kCompleteTaskStmt),
      select_task_stmt_(conn, kSelectTaskStmt),
      select_task_by_identifier_stmt_(conn, kSelectTaskByIdentifierStmt),
      select_worker_id_by_uuid_stmt_(conn, kSelectWorkerIdByUuidStmt),
      update_worker_stmt_(conn, kUpdateWorkerStmt),
      insert_worker_stmt_(conn, kInsertWorkerStmt),
      insert_historic_worker_stmt_(conn, kInsertHistoricWorkerStmt),
      unassign_tasks_stmt_(conn, kUnassignTasksStmt),
      unassign_single_task_stmt_(conn, kUnassignSingleTaskByRowidStmt),
      update_worker_invalidated_uuid_stmt_(conn, kUpdateWorkerInvalidatedUuidStmt),
      select_expired_workers_stmt_(conn, kSelectExpiredWorkersStmt) {
}

StatementsManager::~StatementsManager() {}

DatabaseStatement* StatementsManager::GetOrCreateSelectStmtInExec(const Worker& worker) {
  std::unique_ptr<DatabaseStatement>* found = folly::get_ptr(select_task_stmts_, worker.uuid());
  if (found != nullptr) {
    return found->get();
  }

  if (worker.capabilities_size() == 0) {
    return nullptr;
  }

  std::string tpl = R"(
    select id, task_id, task_type, data, priority, retries, progress_metadata, modules from tasks where
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

  DatabaseStatement* stmt = new DatabaseStatement(conn_, stmt_str);
  select_task_stmts_[worker.uuid()] = std::unique_ptr<DatabaseStatement>(stmt);
  return stmt;
}
}  // tortuga