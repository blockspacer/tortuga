#pragma once

#include <string>

#include "tortuga/tortuga.pb.h"
#include "tortuga/storage/database_statement.h"

namespace tortuga {
class StatementsManager {
 public:
  explicit StatementsManager(sqlite3* db);
  ~StatementsManager();

  SqliteStatement* select_existing_task_stmt() { return &select_existing_task_stmt_; }
  SqliteStatement* insert_task_stmt() { return &insert_task_stmt_; }
  SqliteStatement* assign_task_stmt() { return &assign_task_stmt_; }

  SqliteStatement* select_task_to_complete_stmt() { return &select_task_to_complete_stmt_; }
  SqliteStatement* complete_task_stmt() { return &complete_task_stmt_; }

  // Caller doesn't take ownership.
  // This may return nullptr if the caller has no capabilities. 
  SqliteStatement* GetOrCreateSelectStmtInExec(const Worker& worker);
  // cleanup the prepared statement for a worker if any.
  void Cleanup(const std::string& uuid) {
    select_task_stmts_.erase(uuid);
  }

  SqliteStatement* select_task_stmt() { return &select_task_stmt_; }
  SqliteStatement* select_task_by_identifier_stmt() { return &select_task_by_identifier_stmt_; }
  SqliteStatement* select_worker_id_by_uuid_stmt() { return &select_worker_id_by_uuid_stmt_; }

  SqliteStatement* update_worker_stmt() { return &update_worker_stmt_; };
  SqliteStatement* insert_worker_stmt() { return &insert_worker_stmt_; };
  SqliteStatement* insert_historic_worker_stmt() { return &insert_historic_worker_stmt_; };

  SqliteStatement* unassign_tasks_stmt() { return &unassign_tasks_stmt_; };
  SqliteStatement* unassign_single_task_stmt() { return &unassign_single_task_stmt_; };
  SqliteStatement* update_worker_invalidated_uuid_stmt() { return &update_worker_invalidated_uuid_stmt_; };
  SqliteStatement* select_expired_workers_stmt() { return &select_expired_workers_stmt_; };

 private:
  // not owned
  sqlite3* db_;

  // All our sqlite statements nice and prepared :).
  // Tortuga statements:
  SqliteStatement select_existing_task_stmt_;

  SqliteStatement insert_task_stmt_;
  SqliteStatement assign_task_stmt_;

  SqliteStatement select_task_to_complete_stmt_;
  SqliteStatement complete_task_stmt_;

  // map from worker UUID to its select statement.
  std::map<std::string, std::unique_ptr<SqliteStatement>> select_task_stmts_;

  // progress manager statements:

  SqliteStatement select_task_stmt_;
  SqliteStatement select_task_by_identifier_stmt_;
  SqliteStatement select_worker_id_by_uuid_stmt_;

  // worker manager statements:
  SqliteStatement update_worker_stmt_;
  SqliteStatement insert_worker_stmt_;
  SqliteStatement insert_historic_worker_stmt_;

  SqliteStatement unassign_tasks_stmt_;
  SqliteStatement unassign_single_task_stmt_;
  SqliteStatement update_worker_invalidated_uuid_stmt_;
  SqliteStatement select_expired_workers_stmt_;
};
}
