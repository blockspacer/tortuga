#pragma once

#include <memory>

#include "sqlite3.h"

namespace tortuga {
class TortugaStorage {
 public:
  static std::shared_ptr<TortugaStorage> Init();

  explicit TortugaStorage(sqlite3* db);
  ~TortugaStorage();

  sqlite3* db() const { return db_; }

 private:
  // owned
  sqlite3* db_;
};
}  // tortuga
