#pragma once

#include <memory>
#include <string>

#include "boost/utility.hpp"
#include "cppconn/connection.h"
#include "cppconn/exception.h"
#include "cppconn/prepared_statement.h"
#include "cppconn/resultset.h"
#include "cppconn/statement.h"
#include "google/protobuf/timestamp.pb.h"

namespace tortuga {
class DatabaseStatement {
 public:
  DatabaseStatement(sql::Connection* conn, const std::string& stmt);
  ~DatabaseStatement();

  DatabaseStatement(const DatabaseStatement& other) = delete;
  DatabaseStatement(DatabaseStatement&& other) {
    conn_ = other.conn_;
    stmt_ = other.stmt_;

    other.conn_ = nullptr;
    other.stmt_ = nullptr;
  }

  void BindTimestamp(int pos, const google::protobuf::Timestamp& val);
  void BindText(int pos, const std::string& val);
  void BindBlob(int pos, const std::string& val);
  void BindBool(int pos, bool val);
  void BindInt(int pos, int val);
  void BindLong(int pos, int64_t val);
  void BindFloat(int pos, float val);
  void BindNull(int pos);

  bool Step();
  // Executes a single statement (insert or update) that has no result.
  void ExecuteOrDie();

  bool IsNullColumn(const std::string& pos);
  int ColumnInt(const std::string& pos);
  bool ColumnBool(const std::string& pos);
  int64_t ColumnLong(const std::string& pos);
  float ColumnFloat(const std::string& pos);
  std::string ColumnText(const std::string& pos);
  // If the column is NULL this returns empty.
  std::string ColumnTextOrEmpty(const std::string& pos);
  std::string ColumnBlob(const std::string& pos);
  // If null, returns empty, else the timestamp, parsed as proto.
  std::unique_ptr<google::protobuf::Timestamp> ColumnTimestamp(const std::string& pos);

  void ResetOrDie();

 private:
  // not owned.
  sql::Connection* conn_{ nullptr };
  sql::PreparedStatement* stmt_ { nullptr };

  // Current result set.
  std::unique_ptr<sql::ResultSet> res_;

  // This is a collection of blob streams that must be kept around until execute.
  std::vector<std::unique_ptr<std::istringstream>> streams_;
};

class DatabaseReset : boost::noncopyable {
 public:
  explicit DatabaseReset(DatabaseStatement* stmt);
  ~DatabaseReset();

 private:
  // not owned.
  DatabaseStatement* stmt_{ nullptr };
};
}  // namespace tortuga
