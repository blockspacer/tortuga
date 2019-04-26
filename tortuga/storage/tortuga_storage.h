#pragma once

#include <memory>

#include "folly/Optional.h"
#include "sqlite3.h"

#include "tortuga/storage/sqlite_statement.h"

namespace tortuga {
class TortugaStorage {
 public:
  static std::shared_ptr<TortugaStorage> Init();

  explicit TortugaStorage(sqlite3* db);
  ~TortugaStorage();

  sqlite3* db() const { return db_; }

  folly::Optional<int64_t> FindTaskById(const std::string& id);

 private:
  // owned
  sqlite3* db_;

  SqliteStatement select_existing_task_stmt_;

};
}  // tortuga
