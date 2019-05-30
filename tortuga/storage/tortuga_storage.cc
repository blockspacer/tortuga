#include "tortuga/storage/tortuga_storage.h"

#include <string>

#include "folly/FileUtil.h"
#include "folly/String.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "google/protobuf/text_format.h"
#include "google/protobuf/timestamp.pb.h"
#include "google/protobuf/util/time_util.h"

#include "tortuga/time_utils.h"
#include "tortuga/tortuga.pb.h"
#include "tortuga/storage/fields.h"

DEFINE_bool(truncate_for_test, false, "Truncate tables on startup! WARNING DO NOT USE BUT FOR TESTS");
DEFINE_string(database_conn_file, ".mysql.pbtext", "path to a file that defines params for the mysql connection.");

using google::protobuf::Timestamp;
using google::protobuf::util::TimeUtil;

namespace tortuga {
namespace {
std::string JoinCapabilities(const Worker& worker) {
  return folly::join(" ", worker.capabilities());
}

MysqlParams LoadConnectionParams() {
  std::string content;
  CHECK(folly::readFile(FLAGS_database_conn_file.c_str(), content));

  MysqlParams params;
  CHECK(google::protobuf::TextFormat::ParseFromString(content, &params));
  return params;
}
}  // anonymous

Tx::Tx(sql::Connection* conn) : conn_(conn) {
  // TODO(christian) start TX.
}

Tx::Tx(Tx&& tx) {
  conn_ = tx.conn_;
  tx.conn_ = nullptr;
}

Tx::~Tx() {
  if (conn_ != nullptr) {
    // TODO(christian) end TX.
  }
}

TortugaStorage::TortugaStorage(std::unique_ptr<sql::Connection> conn) : conn_(std::move(conn)) {
  statements_.reset(new StatementsManager(conn_.get()));
}

TortugaStorage::~TortugaStorage() {
}

std::shared_ptr<TortugaStorage> TortugaStorage::Init() {
  try {
    /* Connect to the mySQL instance */
    const auto params = LoadConnectionParams();

    sql::Driver* driver = get_driver_instance();

    sql::ConnectOptionsMap connection_properties;
    connection_properties["hostName"] = params.host();
    connection_properties["userName"] = params.user();
    connection_properties["password"] = params.password();
    connection_properties["schema"] = params.database();
    connection_properties["port"] = params.port();
    connection_properties["OPT_RECONNECT"] = true;

    std::unique_ptr<sql::Connection> conn(driver->connect(connection_properties));
    conn->setSchema(params.database());

    if (FLAGS_truncate_for_test) {
      LOG(INFO) << "Truncating all tables for tests";
      std::vector<std::string> tables = { "tasks", "workers", "historic_workers" };

      for (const auto& table : tables) {
        std::unique_ptr<sql::Statement> truncate_stmt(conn->createStatement());
        truncate_stmt->execute("TRUNCATE TABLE " + table + " ;");
      }
    }

    return std::make_shared<TortugaStorage>(std::move(conn));
  } catch (sql::SQLException& e) {
    LOG(FATAL) << "SQL error: " << e.what() << ". With MYSQL code: " << e.getErrorCode();
  }
}

folly::Optional<int64_t> TortugaStorage::FindTaskById(const std::string& id) {
  auto* select_existing_task_stmt = statements_->select_existing_task_stmt();
  DatabaseReset x(select_existing_task_stmt);
  select_existing_task_stmt->BindText(1, id);
  bool stepped = select_existing_task_stmt->Step();
  if (stepped) {
    int64_t rowid = select_existing_task_stmt->ColumnLong("id");
    return rowid;
  } else {
    return folly::none;
  }
}

int64_t TortugaStorage::InsertTaskNotCommit(const Task& task) {
  auto* insert_task_stmt = statements_->insert_task_stmt();
  DatabaseReset x2(insert_task_stmt);

  insert_task_stmt->BindText(1, task.id());
  insert_task_stmt->BindText(2, task.type());

  std::string data_str;
  CHECK(task.data().SerializeToString(&data_str));
  insert_task_stmt->BindBlob(3, data_str);
   
  int max_retries = 3;
  if (task.has_max_retries()) {
    max_retries = task.max_retries().value();
  }

  insert_task_stmt->BindInt(4, max_retries);

  int priority = 0;
  if (task.has_priority()) {
    priority = task.priority().value();
  }

  insert_task_stmt->BindInt(5, priority);
  if (task.has_delay()) {
    Timestamp now = TimeUtil::GetCurrentTime();
    Timestamp delayed_time = now + task.delay();

    insert_task_stmt->BindTimestamp(6, delayed_time);
  } else {
    insert_task_stmt->BindNull(6);
  }

  if (task.modules_size() == 0) {
    insert_task_stmt->BindNull(7);
  } else {
    std::string modules = folly::join(",", task.modules());
    insert_task_stmt->BindText(7, modules);
  }

  insert_task_stmt->BindTimestamp(8, TimeUtil::GetCurrentTime());

  insert_task_stmt->ExecuteOrDie();

  std::unique_ptr<sql::Statement> last_insert_stmt(conn_->createStatement());
  std::unique_ptr<sql::ResultSet> last_res(last_insert_stmt->executeQuery("SELECT LAST_INSERT_ID();"));
  CHECK(last_res->next());
  return last_res->getInt64(1);
}

void TortugaStorage::AssignNotCommit(int retries, const std::string& worked_uuid, int64_t task_row_id) {
  auto* assign_task_stmt = statements_->assign_task_stmt();
  DatabaseReset x2(assign_task_stmt);
  assign_task_stmt->BindInt(1, retries + 1);
  assign_task_stmt->BindText(2, worked_uuid);
  assign_task_stmt->BindLong(3, CurrentTimeMillis());
  assign_task_stmt->BindLong(4, task_row_id);
  assign_task_stmt->ExecuteOrDie();
}

void TortugaStorage::CompleteTaskNotCommit(int64_t task_id, const CompleteTaskReq& req, bool done) {
  auto* complete_task_stmt = statements_->complete_task_stmt();
  DatabaseReset x2(complete_task_stmt);

  complete_task_stmt->BindInt(1, req.code());
  complete_task_stmt->BindText(2, req.error_message());
  complete_task_stmt->BindBool(3, done);
  complete_task_stmt->BindLong(4, CurrentTimeMillis());  // done_time
  complete_task_stmt->BindText(5, req.logs());
  complete_task_stmt->BindText(6, req.output());
  complete_task_stmt->BindLong(7, task_id);

  complete_task_stmt->ExecuteOrDie();
}

folly::Optional<TaskToComplete> TortugaStorage::SelectTaskToCompleteNotCommit(int64_t task_id) {
  auto* select_task_to_complete_stmt = statements_->select_task_to_complete_stmt();

  DatabaseReset x(select_task_to_complete_stmt);

  select_task_to_complete_stmt->BindLong(1, task_id);
  bool rc = select_task_to_complete_stmt->Step();

  if (!rc) {
    return folly::none;
  }

  TaskToComplete res;
  res.worker_uuid = select_task_to_complete_stmt->ColumnTextOrEmpty("worker_uuid");
  res.max_retries = select_task_to_complete_stmt->ColumnInt("max_retries");
  res.retries = select_task_to_complete_stmt->ColumnInt("retries");
  return res;
}

void TortugaStorage::UpdateProgressNotCommit(int64_t task_id, const UpdateProgressReq& req) {
  std::ostringstream query;
  query << "update tasks set ";

  std::vector<std::string> setters;
  if (req.has_progress()) {
    setters.push_back("progress=?");
  }

  if (req.has_progress_message()) {
    setters.push_back("progress_message=?");    
  }

  if (req.has_progress_metadata()) {
    setters.push_back("progress_metadata=?");    
  }

  query << folly::join(", ", setters);
  query << " where id=? ;";

  std::string query_str = query.str();
  DatabaseStatement stmt(conn_.get(), query_str);

  int idx = 0;
  if (req.has_progress()) {
    stmt.BindFloat(++idx, req.progress().value());
  }

  if (req.has_progress_message()) {
    stmt.BindText(++idx, req.progress_message().value());
  }

  if (req.has_progress_metadata()) {
    stmt.BindText(++idx, req.progress_metadata().value());
  }

  stmt.BindLong(++idx, task_id);

  DatabaseReset x(&stmt);
  stmt.ExecuteOrDie();
}

RequestTaskResult TortugaStorage::RequestTaskNotCommit(const Worker& worker) {
  DatabaseStatement* select_task_stmt = statements_->GetOrCreateSelectStmtInExec(worker);

  if (select_task_stmt == nullptr) {
    VLOG(3) << "This worker: " << worker.uuid() << " has no capabilities!";
    RequestTaskResult res;
    res.none = true;
    return res;
  }

  DatabaseReset x(select_task_stmt);
  Timestamp now = TimeUtil::GetCurrentTime();
  select_task_stmt->BindLong(1, TimeUtil::TimestampToMilliseconds(now));

  bool rc = select_task_stmt->Step();
  if (!rc) {
    VLOG(3) << "Tortuga has no task at the moment";
    RequestTaskResult res;
    res.none = true;
    return res;
  }

  int64_t rowid = select_task_stmt->ColumnLong("id");
  std::string id = select_task_stmt->ColumnText("task_id");
  std::string task_type = select_task_stmt->ColumnText("task_type");
  std::string data = select_task_stmt->ColumnBlob("data");
  int priority = select_task_stmt->ColumnInt("priority");
  int retries = select_task_stmt->ColumnInt("retries");
  std::string progress_metadata = select_task_stmt->ColumnTextOrEmpty("progress_metadata");
  std::string modules = select_task_stmt->ColumnTextOrEmpty("modules");

  RequestTaskResult res;
  res.none = false;
  res.id = id;
  res.handle = rowid;
  res.type = task_type;
  res.data = data;
  res.priority = priority;
  res.retries = retries + 1;
  std::swap(res.progress_metadata, progress_metadata);
  folly::split(",", modules, res.modules);

  return res;
}

UpdatedTask* TortugaStorage::FindUpdatedTaskByHandle(int64_t handle) {
  auto* select_task_stmt = statements_->select_task_stmt();
  select_task_stmt->BindLong(1, handle);

  return FindTaskByBoundStmt(select_task_stmt);
}

UpdatedTask* TortugaStorage::FindUpdatedTask(const TaskIdentifier& t_id) {
  auto* select_task_by_identifier_stmt = statements_->select_task_by_identifier_stmt();

  select_task_by_identifier_stmt->BindText(1, t_id.id());
  select_task_by_identifier_stmt->BindText(2, t_id.type());

  return FindTaskByBoundStmt(select_task_by_identifier_stmt);
}

UpdatedTask* TortugaStorage::FindTaskByBoundStmt(DatabaseStatement* stmt) {
  DatabaseReset x(stmt);

  bool rc = stmt->Step();
  if (!rc) {
    return nullptr;
  }

  TaskProgress res;
  int64_t handle = stmt->ColumnLong("id");
  res.set_handle(folly::to<std::string>(handle));
  res.set_id(stmt->ColumnText("task_id"));
  res.set_type(stmt->ColumnText("task_type"));

  auto created_opt = stmt->ColumnTimestamp("created");
  if (created_opt != nullptr) {
    *res.mutable_created() = *created_opt;
  }

  res.set_max_retries(stmt->ColumnInt("max_retries"));
  res.set_retries(stmt->ColumnInt("retries"));
  res.set_priority(stmt->ColumnInt("priority"));

  std::string modules = stmt->ColumnTextOrEmpty("modules");
  res.set_worked_on(stmt->ColumnBool("worked_on"));

  std::string worker_uuid = stmt->ColumnTextOrEmpty("worker_uuid");

  res.set_progress(stmt->ColumnFloat("progress"));
  res.set_progress_message(stmt->ColumnTextOrEmpty("progress_message"));
  res.set_progress_metadata(stmt->ColumnTextOrEmpty("progress_metadata"));

  res.mutable_status()->set_code(stmt->ColumnInt("status_code"));
  res.mutable_status()->set_message(stmt->ColumnTextOrEmpty("status_message"));

  res.set_done(stmt->ColumnBool("done"));

  auto started_time_opt = stmt->ColumnTimestamp("started_time");
  if (started_time_opt != nullptr) {
    *res.mutable_started_time() = *started_time_opt;
  }

  auto done_time_opt = stmt->ColumnTimestamp("done_time");
  if (done_time_opt != nullptr) {
    *res.mutable_done_time() = *done_time_opt;
  }

  res.set_logs(stmt->ColumnTextOrEmpty("logs"));
  res.set_output(stmt->ColumnTextOrEmpty("output"));

  if (!worker_uuid.empty()) {
    auto* select_worker_id_by_uuid_stmt = statements_->select_worker_id_by_uuid_stmt();
    DatabaseReset x2(select_worker_id_by_uuid_stmt);
    select_worker_id_by_uuid_stmt->BindText(1, worker_uuid);
    
    // This shall always be true because if a task has a worker uuid then that worker must be in the historic table.
    if (select_worker_id_by_uuid_stmt->Step()) {
      res.set_worker_id(select_worker_id_by_uuid_stmt->ColumnText("worker_id"));
    }
  }

  UpdatedTask* updated_task = new UpdatedTask();
  updated_task->progress = std::make_unique<TaskProgress>(res);
  folly::split(",", modules, updated_task->modules);

  return updated_task;
}

void TortugaStorage::UpdateNewWorkerNotCommit(const Worker& worker) {
  auto* update_worker_stmt = statements_->update_worker_stmt();

  DatabaseReset x(update_worker_stmt);
  update_worker_stmt->BindText(1, worker.uuid());
  update_worker_stmt->BindText(2, JoinCapabilities(worker));
  update_worker_stmt->BindLong(3, CurrentTimeMillis());
  update_worker_stmt->BindText(4, worker.worker_id());

  update_worker_stmt->ExecuteOrDie();
}

void TortugaStorage::InsertWorkerNotCommit(const Worker& worker) {
  auto* insert_worker_stmt = statements_->insert_worker_stmt();
  
  DatabaseReset x(insert_worker_stmt);
  
  insert_worker_stmt->BindText(1, worker.uuid());
  insert_worker_stmt->BindText(2, worker.worker_id());
  insert_worker_stmt->BindText(3, JoinCapabilities(worker));
  insert_worker_stmt->BindLong(4, CurrentTimeMillis());
  insert_worker_stmt->ExecuteOrDie();
}

void TortugaStorage::InsertHistoricWorkerNotCommit(const std::string& uuid,
                                                   const std::string& worker_id) {
  auto* insert_historic_worker_stmt = statements_->insert_historic_worker_stmt();
                                                  
  DatabaseReset x(insert_historic_worker_stmt);
  insert_historic_worker_stmt->BindText(1, uuid);
  insert_historic_worker_stmt->BindText(2, worker_id);
  insert_historic_worker_stmt->BindLong(3, CurrentTimeMillis());

  insert_historic_worker_stmt->ExecuteOrDie();
}

void TortugaStorage::UnassignTasksOfWorkerNotCommit(const std::string& uuid) {
  auto* unassign_tasks_stmt = statements_->unassign_tasks_stmt();

  DatabaseReset x(unassign_tasks_stmt);
  unassign_tasks_stmt->BindText(1, uuid);
  unassign_tasks_stmt->ExecuteOrDie();
}

void TortugaStorage::InvalidateExpiredWorkerNotCommit(const std::string& uuid) {
  auto* update_worker_invalidated_uuid_stmt = statements_->update_worker_invalidated_uuid_stmt();

  DatabaseReset x(update_worker_invalidated_uuid_stmt);
  update_worker_invalidated_uuid_stmt->BindText(1, uuid);
  update_worker_invalidated_uuid_stmt->BindText(2, uuid);
  update_worker_invalidated_uuid_stmt->ExecuteOrDie();
}

void TortugaStorage::UnassignTaskNotCommit(int64_t handle) {
  auto* unassign_single_task_stmt = statements_->unassign_single_task_stmt();

  DatabaseReset x(unassign_single_task_stmt);
  unassign_single_task_stmt->BindLong(1, handle);
  unassign_single_task_stmt->ExecuteOrDie();
}

namespace {
static const char* const kSelectWorkerByUuidStmt = R"(
  select * from workers where uuid = ?;
)" ;
}

folly::Optional<std::string> TortugaStorage::FindWorkerIdByUuidUnprepared(const std::string& uuid) {
  DatabaseStatement select_task_worker(conn_.get(), kSelectWorkerByUuidStmt);
  select_task_worker.BindText(1, uuid);
  bool found_worker = select_task_worker.Step();
  if (!found_worker) {
    return folly::none;
  } else {
    return select_task_worker.ColumnText("worker_id");
  }
}

namespace {
static const char* const kSelectTasksWorkedOnStmt = R"(
  select * from tasks where worked_on = 1 and done = 0;
)";
}

TasksWorkedOnIterator::TasksWorkedOnIterator(sql::Connection* conn) : conn_(conn), tasks_(conn, kSelectTasksWorkedOnStmt) {
}

folly::Optional<TaskWorkedOn> TasksWorkedOnIterator::Next() {
  bool stepped = tasks_.Step();

  if (!stepped) {
    return folly::none;
  }

  TaskWorkedOn res;
  res.row_id = tasks_.ColumnLong("id");
  res.worker_uuid = tasks_.ColumnTextOrEmpty("worker_uuid");

  return res;
}

TasksWorkedOnIterator TortugaStorage::IterateTasksWorkedOn() {
  return TasksWorkedOnIterator(conn_.get());
}
}  // tortuga
