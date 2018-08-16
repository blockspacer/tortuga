#pragma once

#include <map>
#include <string>
#include <vector>

#include <boost/utility.hpp>
#include "folly/executors/CPUThreadPoolExecutor.h" 
#include "folly/fibers/FiberManager.h"
#include "grpc++/grpc++.h"
#include "sqlite3.h"

#include "tortuga/sqlite_statement.h"
#include "tortuga/tortuga.grpc.pb.h"
#include "tortuga/tortuga.pb.h"

namespace tortuga {
struct RpcOpts {
  Tortuga::AsyncService* tortuga_grpc{ nullptr };
  grpc::ServerCompletionQueue* cq{ nullptr };
  folly::fibers::FiberManager* fibers{ nullptr };
};

struct RegisteredWorker {
  int client_id{ 0 };

  std::string process_id;
  std::string uuid;

  std::vector<std::string> capabilities;
};

class TortugaHandler : boost::noncopyable {
 public:
  explicit TortugaHandler(sqlite3* db);
  void HandleCreateTask(RpcOpts opts);
  void HandleRequestTask(RpcOpts opts);
  void HandleHeartbeat(RpcOpts opts);
  void HandleCompleteTask(RpcOpts opts);


  // admin commands.
  void HandlePing(RpcOpts opts);
  void HandleQuit(RpcOpts opts);

  void CheckHeartbeatsLoop();

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

  void CompleteTask(const CompleteTaskReq& req);
  void CompleteTaskInExec(const CompleteTaskReq& req);

  std::vector<std::string> ExpiredWorkersInExec();
  void UnassignTasksInExec(const std::vector<std::string>& uuids);
  void UnassignTaskInExec(const std::string& uuid);

  sqlite3* db_{ nullptr };

  // executor in which we perform sqlite tasks.
  folly::CPUThreadPoolExecutor exec_{ 1, 1, 1024 };

  // All our sqlite statements nice and prepared :).

  SqliteStatement select_existing_task_stmt_;
  SqliteStatement insert_task_stmt_;
  SqliteStatement select_worker_uuid_stmt_;
  SqliteStatement update_worker_beat_stmt_;
  SqliteStatement update_worker_stmt_;
  SqliteStatement insert_worker_stmt_;
  SqliteStatement select_task_stmt_;
  SqliteStatement assign_task_stmt_;

  SqliteStatement select_task_to_complete_stmt_;
  SqliteStatement complete_task_stmt_;

  SqliteStatement select_expired_workers_stmt_;

  SqliteStatement unassign_tasks_stmt_;
  SqliteStatement update_worker_invalidated_uuid_stmt_;
};
}  // namespace tortuga
