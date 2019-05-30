#pragma once

#include <memory>

#include "cppconn/driver.h"
#include "cppconn/exception.h"
#include "cppconn/resultset.h"
#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"
#include "folly/Optional.h"

#include "tortuga/request_task_result.h"
#include "tortuga/tortuga.pb.h"
#include "tortuga/updated_task.h"
#include "tortuga/storage/statements_manager.h"

namespace tortuga {
struct TaskToComplete {
  std::string worker_uuid;
  int max_retries{ 0 };
  int retries{ 0 };
};

struct TaskWorkedOn {
  int64_t row_id{ 0 };
  std::string worker_uuid;
};

class TasksWorkedOnIterator {
 public:
  explicit TasksWorkedOnIterator(sql::Connection* conn);

  folly::Optional<TaskWorkedOn> Next();

 private:
  sql::Connection* conn_{ nullptr };
  DatabaseStatement tasks_;
};

class TortugaStorage;

class Tx {
 public:
  Tx(Tx&& tx);
  ~Tx();

 private:
  friend class TortugaStorage;

  explicit Tx(sql::Connection* conn);
  Tx(const Tx& tx) = delete;
  Tx(Tx& tx) = delete;
 
  sql::Connection* conn_{ nullptr };
};

class TortugaStorage {
 public:
  static std::shared_ptr<TortugaStorage> Init();

  explicit TortugaStorage(std::unique_ptr<sql::Connection> conn);
  ~TortugaStorage();

  Tx StartTx() {
    return Tx(conn_.get());
  }

  folly::Optional<int64_t> FindTaskById(const std::string& id);
  int64_t InsertTaskNotCommit(const Task& task);

  void AssignNotCommit(int retries, const std::string& worked_uuid, int64_t task_row_id);
  void CompleteTaskNotCommit(int64_t task_id, const CompleteTaskReq& req, bool done);

  folly::Optional<TaskToComplete> SelectTaskToCompleteNotCommit(int64_t task_id);

  void UpdateProgressNotCommit(int64_t task_id, const UpdateProgressReq& req);

  void Cleanup(const std::string& uuid) {
    statements_->Cleanup(uuid);
  }

  RequestTaskResult RequestTaskNotCommit(const Worker& worker);

  // Caller takes ownership!
  UpdatedTask* FindUpdatedTaskByHandle(int64_t handle);
  UpdatedTask* FindUpdatedTask(const TaskIdentifier& t_id);

  // workers
  void UpdateNewWorkerNotCommit(const Worker& worker);
  void InsertWorkerNotCommit(const Worker& worker);
  void InsertHistoricWorkerNotCommit(const std::string& uuid,
                                     const std::string& worker_id);
  void UnassignTasksOfWorkerNotCommit(const std::string& uuid);
  void InvalidateExpiredWorkerNotCommit(const std::string& uuid);
  void UnassignTaskNotCommit(int64_t handle);

  folly::Optional<std::string> FindWorkerIdByUuidUnprepared(const std::string& uuid);

  TasksWorkedOnIterator IterateTasksWorkedOn();

 private:
  UpdatedTask* FindTaskByBoundStmt(DatabaseStatement* stmt);

  std::unique_ptr<sql::Connection> conn_;

  std::unique_ptr<StatementsManager> statements_;
};
}  // tortuga
