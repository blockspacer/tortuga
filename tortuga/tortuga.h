#pragma once

#include <chrono>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "boost/utility.hpp"
#include "folly/executors/CPUThreadPoolExecutor.h" 
#include "folly/fibers/Baton.h" 
#include "grpc++/grpc++.h"
#include "sqlite3.h"

#include "tortuga/module.h"
#include "tortuga/progress_manager.h"
#include "tortuga/rpc_opts.h"
#include "tortuga/sqlite_statement.h"
#include "tortuga/tortuga.grpc.pb.h"
#include "tortuga/tortuga.pb.h"
#include "tortuga/workers_manager.h"

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
  struct CreateTaskResult {
    bool created{ false };
    std::string handle;
  };

  CreateTaskResult CreateTask(const Task& task);
  CreateTaskResult CreateTaskInExec(const Task& task);

  struct RequestTaskResult {
    bool none { false };
    std::string id;
    int64_t handle{ 0 };
    std::string type;
    std::string data;
    int priority{ 0 };
    int retries{ 0 };
    std::string progress_metadata;
  };

  RequestTaskResult RequestTask(const Worker& worker,
                                std::chrono::system_clock::time_point rpc_exp,
                                bool first_try);
  RequestTaskResult RequestTaskInExec(const Worker& worker);

  UpdatedTask* CompleteTask(const CompleteTaskReq& req);
  UpdatedTask* CompleteTaskInExec(const CompleteTaskReq& req);

  UpdatedTask* UpdateProgress(const UpdateProgressReq& req);
  UpdatedTask* UpdateProgressInExec(const UpdateProgressReq& req);

  void MaybeNotifyModulesOfUpdate(const UpdatedTask& task);
  void MaybeNotifyModulesOfCreation(const std::string& handle,
                                    const std::vector<std::string> modules);
  void UpdateProgressManagerCache(const UpdatedTask& task);

  // Caller doesn't take ownership.
  // This may return nullptr if the caller has no capabilities. 
  SqliteStatement* GetOrCreateSelectStmtInExec(const Worker& worker);

  void RegisterWaitingWorker(const Worker& worker, folly::fibers::Baton* baton);
  void UnregisterWaitingWorker(const Worker& worker, folly::fibers::Baton* baton);

  sqlite3* db_{ nullptr };
  RpcOpts rpc_opts_;

  // executor in which we perform sqlite tasks.
  folly::CPUThreadPoolExecutor exec_{ 1, 1, 8192 };

  // progress manager
  std::unique_ptr<ProgressManager> progress_mgr_;

  // All our sqlite statements nice and prepared :).

  SqliteStatement select_existing_task_stmt_;
  SqliteStatement insert_task_stmt_;
  SqliteStatement assign_task_stmt_;

  SqliteStatement select_task_to_complete_stmt_;
  SqliteStatement complete_task_stmt_;

  // map from worker UUID to its select statement.
  std::map<std::string, std::unique_ptr<SqliteStatement>> select_task_stmts_;

  // all modules
  const std::map<std::string, std::unique_ptr<Module>> modules_;

  std::unique_ptr<WorkersManager> workers_manager_;

  // Map from task type to batons that are waiting for that task.
  std::map<std::string, std::set<folly::fibers::Baton*>> waiting_for_tasks_;
};
}  // namespace tortuga
