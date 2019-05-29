#include "tortuga/storage/statements_manager.h"

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
}  // anonymous

StatementsManager::StatementsManager(sqlite3* db)
    : select_existing_task_stmt_(db, kSelectExistingTaskStmt),
      insert_task_stmt_(db, kInsertTaskStmt),
      assign_task_stmt_(db, kAssignTaskStmt),
      select_task_to_complete_stmt_(db, kSelectTaskToCompleteStmt),
      complete_task_stmt_(db, kCompleteTaskStmt) {
}

StatementsManager::~StatementsManager() {}
}  // tortuga