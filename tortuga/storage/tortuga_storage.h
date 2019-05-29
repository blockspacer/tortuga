#pragma once

#include <memory>

#include "folly/Optional.h"
#include "sqlite3.h"

#include "tortuga/tortuga.pb.h"
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

 private:
  // owned
  sqlite3* db_;

  std::unique_ptr<StatementsManager> statements_;
};
}  // tortuga
