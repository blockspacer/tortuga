#pragma once

#include <string>

#include "boost/utility.hpp"
#include "sqlite3.h"

namespace tortuga {
class SqliteStatement : boost::noncopyable {
 public:
  SqliteStatement(sqlite3* db, const std::string& stmt);
  ~SqliteStatement();

  void BindText(int pos, const std::string& val);
  void BindBlob(int pos, const std::string& val);
  void BindBool(int pos, bool val);
  void BindInt(int pos, int val);
  void BindLong(int pos, int64_t val);

  int Step();
  // Executes a single statement (insert or update) that has no result.
  void ExecuteOrDie();

  bool IsNullColumn(int pos);
  int ColumnInt(int pos);
  int64_t ColumnLong(int pos);
  std::string ColumnText(int pos);
  // If the column is NULL this returns empty.
  std::string ColumnTextOrEmpty(int pos);
  std::string ColumnBlob(int pos);

  void ResetOrDie();

 private:
  // not owned.
  sqlite3* db_{ nullptr };
  sqlite3_stmt* stmt_ { nullptr };
};

class SqliteTx : boost::noncopyable {
 public:
  explicit SqliteTx(sqlite3* db);
  ~SqliteTx();

 private:
  // not owned.
  sqlite3* db_{ nullptr };
};

class SqliteReset : boost::noncopyable {
 public:
  explicit SqliteReset(SqliteStatement* stmt);
  ~SqliteReset();

 private:
  // not owned.
  SqliteStatement* stmt_{ nullptr };
};
}  // namespace tortuga
