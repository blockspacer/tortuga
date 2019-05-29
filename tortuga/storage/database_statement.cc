#include "tortuga/storage/database_statement.h"

#include "glog/logging.h"

#include "tortuga/time_utils.h"

namespace tortuga {
DatabaseStatement::DatabaseStatement(sqlite3* db, const std::string& stmt) {
  db_ = db;
  int rc = sqlite3_prepare(db, stmt.c_str(), strlen(stmt.c_str()), &stmt_, nullptr);
  CHECK_EQ(SQLITE_OK, rc) << sqlite3_errmsg(db) << "\nstatement was: " << stmt;
}

DatabaseStatement::~DatabaseStatement() {
  if (db_ != nullptr && stmt_ != nullptr) {
    CHECK_EQ(SQLITE_OK, sqlite3_finalize(stmt_));
  }  // else we were probably moved.
}

void DatabaseStatement::BindText(int pos, const std::string& val) {
  int rc = sqlite3_bind_text(stmt_, pos, val.c_str(), val.size(), SQLITE_TRANSIENT);
  CHECK_EQ(SQLITE_OK, rc) << sqlite3_errmsg(db_);
}

void DatabaseStatement::BindBlob(int pos, const std::string& val) {
  int rc = sqlite3_bind_blob(stmt_, pos, val.data(), val.size(), SQLITE_TRANSIENT);
  CHECK_EQ(SQLITE_OK, rc) << sqlite3_errmsg(db_);;
}

void DatabaseStatement::BindBool(int pos, bool val) {
  BindInt(pos, val ? 1 : 0);
}

void DatabaseStatement::BindInt(int pos, int val) {
  int rc = sqlite3_bind_int(stmt_, pos, val);
  CHECK_EQ(SQLITE_OK, rc);
}

void DatabaseStatement::BindLong(int pos, int64_t val) {
  int rc = sqlite3_bind_int64(stmt_, pos, val);
  CHECK_EQ(SQLITE_OK, rc);
}

void DatabaseStatement::BindFloat(int pos, float val) {
  int rc = sqlite3_bind_double(stmt_, pos, static_cast<double>(val));
  CHECK_EQ(SQLITE_OK, rc);
}

void DatabaseStatement::BindNull(int pos) {
  int rc = sqlite3_bind_null(stmt_, pos);
  CHECK_EQ(SQLITE_OK, rc);
}

int DatabaseStatement::Step() {
  return sqlite3_step(stmt_);
}

void DatabaseStatement::ExecuteOrDie() {
  CHECK_EQ(SQLITE_DONE, Step()) << sqlite3_errmsg(db_);
}

bool DatabaseStatement::IsNullColumn(int pos) {
  return sqlite3_column_type(stmt_, pos) == SQLITE_NULL;
}

int DatabaseStatement::ColumnInt(int pos) {
  return sqlite3_column_int(stmt_, pos);
}

bool DatabaseStatement::ColumnBool(int pos) {
  int val = ColumnInt(pos);
  return val != 0;
}

int64_t DatabaseStatement::ColumnLong(int pos) {
  return sqlite3_column_int64(stmt_, pos);
}

float DatabaseStatement::ColumnFloat(int pos) {
  return static_cast<float>(sqlite3_column_double(stmt_, pos));
}

std::string DatabaseStatement::ColumnText(int pos) {
  CHECK(!IsNullColumn(pos));
  const char* text_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, pos));
  return std::string(text_str);
}

std::string DatabaseStatement::ColumnTextOrEmpty(int pos) {
  if (IsNullColumn(pos)) {
    return "";
  }

  return ColumnText(pos);
}

std::unique_ptr<google::protobuf::Timestamp> DatabaseStatement::ColumnTimestamp(int pos) {
  if (IsNullColumn(pos)) {
    return nullptr;
  }

  int64_t millis = ColumnLong(pos);
  return std::make_unique<google::protobuf::Timestamp>(FromEpochMillis(millis));
}

std::string DatabaseStatement::ColumnBlob(int pos) {
  const void* data = sqlite3_column_blob(stmt_, pos);
  int size = sqlite3_column_bytes(stmt_, pos);
  return std::string(reinterpret_cast<const char*>(data), size);
}

void DatabaseStatement::ResetOrDie() {
  CHECK_EQ(SQLITE_OK, sqlite3_reset(stmt_));
  CHECK_EQ(SQLITE_OK, sqlite3_clear_bindings(stmt_));
}

SqliteReset::SqliteReset(DatabaseStatement* stmt) : stmt_(stmt) {
}

SqliteReset::~SqliteReset() {
  stmt_->ResetOrDie();
}
}  // namespace tortuga
