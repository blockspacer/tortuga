#include "tortuga/sqlite_statement.h"

#include "glog/logging.h"

namespace tortuga {
SqliteStatement::SqliteStatement(sqlite3* db, const std::string& stmt) {
  db_ = db;
  int rc = sqlite3_prepare(db, stmt.c_str(), strlen(stmt.c_str()), &stmt_, nullptr);
  CHECK_EQ(SQLITE_OK, rc) << sqlite3_errmsg(db);
}

SqliteStatement::~SqliteStatement() {
  CHECK_EQ(SQLITE_OK, sqlite3_finalize(stmt_));
}

void SqliteStatement::BindText(int pos, const std::string& val) {
  int rc = sqlite3_bind_text(stmt_, pos, val.c_str(), val.size(), SQLITE_TRANSIENT);
  CHECK_EQ(SQLITE_OK, rc) << sqlite3_errmsg(db_);
}

void SqliteStatement::BindBlob(int pos, const std::string& val) {
  int rc = sqlite3_bind_blob(stmt_, pos, val.data(), val.size(), SQLITE_TRANSIENT);
  CHECK_EQ(SQLITE_OK, rc) << sqlite3_errmsg(db_);;
}

void SqliteStatement::BindBool(int pos, bool val) {
  BindInt(pos, val ? 1 : 0);
}

void SqliteStatement::BindInt(int pos, int val) {
  int rc = sqlite3_bind_int(stmt_, pos, val);
  CHECK_EQ(SQLITE_OK, rc);
}

void SqliteStatement::BindLong(int pos, int64_t val) {
  int rc = sqlite3_bind_int64(stmt_, pos, val);
  CHECK_EQ(SQLITE_OK, rc);
}

int SqliteStatement::Step() {
  return sqlite3_step(stmt_);
}

void SqliteStatement::ExecuteOrDie() {
  CHECK_EQ(SQLITE_DONE, Step()) << sqlite3_errmsg(db_);
}

bool SqliteStatement::IsNullColumn(int pos) {
  return sqlite3_column_type(stmt_, pos) == SQLITE_NULL;
}

int SqliteStatement::ColumnInt(int pos) {
  return sqlite3_column_int(stmt_, pos);
}

int64_t SqliteStatement::ColumnLong(int pos) {
  return sqlite3_column_int64(stmt_, pos);
}

std::string SqliteStatement::ColumnText(int pos) {
  CHECK(!IsNullColumn(pos));
  const char* text_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, pos));
  return std::string(text_str);
}

std::string SqliteStatement::ColumnTextOrEmpty(int pos) {
  if (IsNullColumn(pos)) {
    return "";
  }

  return ColumnText(pos);
}

std::string SqliteStatement::ColumnBlob(int pos) {
  const void* data = sqlite3_column_blob(stmt_, pos);
  int size = sqlite3_column_bytes(stmt_, pos);
  return std::string(reinterpret_cast<const char*>(data), size);
}

void SqliteStatement::ResetOrDie() {
  CHECK_EQ(SQLITE_OK, sqlite3_reset(stmt_));
  CHECK_EQ(SQLITE_OK, sqlite3_clear_bindings(stmt_));
}

SqliteTx::SqliteTx(sqlite3* db) : db_(db) {
  CHECK_EQ(SQLITE_OK, sqlite3_exec(db, "begin transaction;", nullptr, nullptr, nullptr));
}

SqliteTx::~SqliteTx() {
  CHECK_EQ(SQLITE_OK, sqlite3_exec(db_, "end transaction;", nullptr, nullptr, nullptr));
}

SqliteReset::SqliteReset(SqliteStatement* stmt) : stmt_(stmt) {
}

SqliteReset::~SqliteReset() {
  stmt_->ResetOrDie();
}
}  // namespace tortuga
