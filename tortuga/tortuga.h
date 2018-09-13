#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "boost/utility.hpp"
#include "folly/executors/CPUThreadPoolExecutor.h" 
#include "grpc++/grpc++.h"
#include "sqlite3.h"

#include "tortuga/module.h"
#include "tortuga/progress_manager.h"
#include "tortuga/rpc_opts.h"
#include "tortuga/sqlite_statement.h"
#include "tortuga/tortuga.grpc.pb.h"
#include "tortuga/tortuga.pb.h"

namespace tortuga {
struct RegisteredWorker {
  int client_id{ 0 };

  std::string process_id;
  std::string uuid;

  std::vector<std::string> capabilities;
};

class TortugaHandler : boost::noncopyable {
 public:
  TortugaHandler(sqlite3* db, RpcOpts rpc_opts, std::map<std::string, std::unique_ptr<Module>> modules);
  ~TortugaHandler() {
  }

  void HandleCreateTask();
  void HandleRequestTask();
  void HandleHeartbeat();
  void HandleCompleteTask();
  void HandleUpdateProgress();

  // admin commands.
  void HandlePing();
  void HandleQuit();

  void CheckHeartbeatsLoop();

  void HandleFindTask() {
    progress_mgr_->HandleFindTask();
  }

  void HandleFindTaskByHandle() {
    progress_mgr_->HandleFindTaskByHandle();
  }

 private:
  void CheckHeartbeats();
 
  struct CreateTaskResult {
    bool created{ false };
    std::string handle;
  };

  CreateTaskResult CreateTask(const Task& task);
  CreateTaskResult CreateTaskInExec(const Task& task);

  void MaybeUpdateWorker(const Worker& worker);
  void MaybeUpdateWorkerInExec(const Worker& worker);

  struct RequestTaskResult {
    bool none { false };
    std::string id;
    std::string handle;
    std::string type;
    std::string data;
  };

  RequestTaskResult RequestTask(const Worker& worker);
  RequestTaskResult RequestTaskInExec(const Worker& worker);

  UpdatedTask* CompleteTask(const CompleteTaskReq& req);
  UpdatedTask* CompleteTaskInExec(const CompleteTaskReq& req);

  UpdatedTask* UpdateProgress(const UpdateProgressReq& req);
  UpdatedTask* UpdateProgressInExec(const UpdateProgressReq& req);

  void MaybeNotifyModules(const UpdatedTask& task);

  std::vector<std::string> ExpiredWorkersInExec();
  void UnassignTasksInExec(const std::vector<std::string>& uuids);
  void UnassignTaskInExec(const std::string& uuid);

  void InsertHistoricWorkerInExec(const std::string& uuid,
                                  const std::string& worker_id);

  // Caller doesn't take ownership.
  // This may return nullptr if the caller has no capabilities. 
  SqliteStatement* GetOrCreateSelectStmtInExec(const Worker& worker);

  sqlite3* db_{ nullptr };
  RpcOpts rpc_opts_;

  // executor in which we perform sqlite tasks.
  folly::CPUThreadPoolExecutor exec_{ 1, 1, 1024 };

  // progress manager
  std::unique_ptr<ProgressManager> progress_mgr_;

  // All our sqlite statements nice and prepared :).

  SqliteStatement select_existing_task_stmt_;
  SqliteStatement insert_task_stmt_;
  SqliteStatement select_worker_uuid_stmt_;
  SqliteStatement update_worker_beat_stmt_;
  SqliteStatement update_worker_stmt_;
  SqliteStatement insert_worker_stmt_;
  SqliteStatement assign_task_stmt_;

  SqliteStatement select_task_to_complete_stmt_;
  SqliteStatement complete_task_stmt_;
  SqliteStatement update_task_progress_stmt_;
  SqliteStatement update_task_progress_only_stmt_;
  SqliteStatement update_task_progress_msg_only_stmt_;

  SqliteStatement select_expired_workers_stmt_;

  SqliteStatement unassign_tasks_stmt_;
  SqliteStatement update_worker_invalidated_uuid_stmt_;

  SqliteStatement insert_historic_worker_stmt_;

  // map from worker UUID to its select statement.
  std::map<std::string, std::unique_ptr<SqliteStatement>> select_task_stmts_;

  // all modules
  const std::map<std::string, std::unique_ptr<Module>> modules_;
};
}  // namespace tortuga
