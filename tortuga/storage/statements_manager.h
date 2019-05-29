#pragma once

#include "tortuga/storage/sqlite_statement.h"

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

 private:
  // All our sqlite statements nice and prepared :).
  // Tortuga statements:
  SqliteStatement select_existing_task_stmt_;

  SqliteStatement insert_task_stmt_;
  SqliteStatement assign_task_stmt_;

  SqliteStatement select_task_to_complete_stmt_;
  SqliteStatement complete_task_stmt_;

  // progress manager statements:

};
}
