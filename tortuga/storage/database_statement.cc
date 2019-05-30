#include "tortuga/storage/database_statement.h"

#include "glog/logging.h"
#include "google/protobuf/util/time_util.h"

#include "tortuga/time_utils.h"

namespace tortuga {
DatabaseStatement::DatabaseStatement(sql::Connection* conn, const std::string& stmt) {
  conn_ = conn;
  stmt_ = conn_->prepareStatement(stmt);
  CHECK(stmt_ != nullptr);
}

DatabaseStatement::~DatabaseStatement() {
  if (conn_ != nullptr && stmt_ != nullptr) {
    delete stmt_;
  }  // else we were probably moved.
}

void DatabaseStatement::BindText(int pos, const std::string& val) {
  try {
    stmt_->setString(pos, val);
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

void DatabaseStatement::BindBlob(int pos, const std::string& val) {
  try {
    auto* is = new std::istringstream(val);
    streams_.emplace_back(is);
    stmt_->setBlob(pos, is);
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

void DatabaseStatement::BindBool(int pos, bool val) {
  try {
    stmt_->setBoolean(pos, val);
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

void DatabaseStatement::BindInt(int pos, int val) {
  try {
    stmt_->setInt(pos, val);
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

void DatabaseStatement::BindLong(int pos, int64_t val) {
  try {
    stmt_->setInt64(pos, val);
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

void DatabaseStatement::BindTimestamp(int pos, const google::protobuf::Timestamp& val) {
  try {
    stmt_->setInt64(pos, google::protobuf::util::TimeUtil::TimestampToSeconds(val));
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

void DatabaseStatement::BindFloat(int pos, float val) {
  try {
    stmt_->setDouble(pos, val);
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

void DatabaseStatement::BindNull(int pos) {
  try {
    stmt_->setNull(pos, 0);
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

bool DatabaseStatement::Step() {
  try {
    if (res_ != nullptr) {
      bool more = res_->next();
      if (!more) {
        res_.reset(nullptr);
        return false;
      } else {
        return true;
      }
    } else {
      res_.reset(stmt_->executeQuery());
      return Step();
    }
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

void DatabaseStatement::ExecuteOrDie() {
  try {
    stmt_->executeUpdate();
    streams_.clear();
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

bool DatabaseStatement::IsNullColumn(const std::string& pos) {
  try {
    CHECK(res_ != nullptr);
    return res_->isNull(pos);
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

int DatabaseStatement::ColumnInt(const std::string& pos) {
  try {
    CHECK(res_ != nullptr);
    return res_->getInt(pos);
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode()
              << ". Pos was: " << pos;
  }
}

bool DatabaseStatement::ColumnBool(const std::string& pos) {
  try {
    CHECK(res_ != nullptr);
    return res_->getBoolean(pos);
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

int64_t DatabaseStatement::ColumnLong(const std::string& pos) {
  try {
    CHECK(res_ != nullptr);
    return res_->getInt64(pos);
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

float DatabaseStatement::ColumnFloat(const std::string& pos) {
  try {
    CHECK(res_ != nullptr);
    return static_cast<float>(res_->getDouble(pos));
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

std::string DatabaseStatement::ColumnText(const std::string& pos) {
  CHECK(res_ != nullptr);
  CHECK(!IsNullColumn(pos));
  return std::string(res_->getString(pos));
}

std::string DatabaseStatement::ColumnTextOrEmpty(const std::string& pos) {
  if (IsNullColumn(pos)) {
    return "";
  }

  return ColumnText(pos);
}

std::unique_ptr<google::protobuf::Timestamp> DatabaseStatement::ColumnTimestamp(const std::string& pos) {
  if (IsNullColumn(pos)) {
    return nullptr;
  }

  std::string time = ColumnTextOrEmpty(pos);

  struct tm tmlol;
  strptime(time.c_str(), "%Y-%m-%d %H:%M:%S", &tmlol);
  time_t t = mktime(&tmlol);
  
  return std::make_unique<google::protobuf::Timestamp>(google::protobuf::util::TimeUtil::TimeTToTimestamp(t));
}

std::string DatabaseStatement::ColumnBlob(const std::string& pos) {
  CHECK(res_ != nullptr);
  auto* stream = res_->getBlob(pos);

  std::ostringstream os;
  os << stream->rdbuf();
  
  return os.str();
}

void DatabaseStatement::ResetOrDie() {
  stmt_->clearParameters();
  res_.reset(nullptr);
  streams_.clear();
}

DatabaseReset::DatabaseReset(DatabaseStatement* stmt) : stmt_(stmt) {
}

DatabaseReset::~DatabaseReset() {
  stmt_->ResetOrDie();
}
}  // namespace tortuga
