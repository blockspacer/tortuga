#pragma once

#include <memory>

#include "folly/Optional.h"
#include "sqlite3.h"

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

class TortugaStorage {
 public:
  static std::shared_ptr<TortugaStorage> Init();

  explicit TortugaStorage(sqlite3* db);
  ~TortugaStorage();

  sqlite3* db() const { return db_; }

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

 private:
  UpdatedTask* FindTaskByBoundStmt(SqliteStatement* stmt);

  // owned
  sqlite3* db_;

  std::unique_ptr<StatementsManager> statements_;
};
}  // tortuga
