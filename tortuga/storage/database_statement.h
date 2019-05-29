#pragma once

#include <memory>
#include <string>

#include "boost/utility.hpp"
#include "google/protobuf/timestamp.pb.h"
#include "sqlite3.h"

namespace tortuga {
class SqliteStatement {
 public:
  SqliteStatement(sqlite3* db, const std::string& stmt);
  ~SqliteStatement();

  SqliteStatement(const SqliteStatement& other) = delete;
  SqliteStatement(SqliteStatement&& other) {
    db_ = other.db_;
    stmt_ = other.stmt_;

    other.db_ = nullptr;
    other.stmt_ = nullptr;
  }

  void BindText(int pos, const std::string& val);
  void BindBlob(int pos, const std::string& val);
  void BindBool(int pos, bool val);
  void BindInt(int pos, int val);
  void BindLong(int pos, int64_t val);
  void BindFloat(int pos, float val);
  void BindNull(int pos);

  int Step();
  // Executes a single statement (insert or update) that has no result.
  void ExecuteOrDie();

  bool IsNullColumn(int pos);
  int ColumnInt(int pos);
  bool ColumnBool(int pos);
  int64_t ColumnLong(int pos);
  float ColumnFloat(int pos);
  std::string ColumnText(int pos);
  // If the column is NULL this returns empty.
  std::string ColumnTextOrEmpty(int pos);
  std::string ColumnBlob(int pos);
  // If null, returns empty, else the timestamp, parsed as proto.
  std::unique_ptr<google::protobuf::Timestamp> ColumnTimestamp(int pos);

  void ResetOrDie();

 private:
  // not owned.
  sqlite3* db_{ nullptr };
  sqlite3_stmt* stmt_ { nullptr };
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
