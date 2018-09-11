#pragma once

#include <map>
#include <vector>

#include "boost/utility.hpp"
#include "folly/executors/CPUThreadPoolExecutor.h"
#include "grpc++/grpc++.h"
#include "sqlite3.h"

#include "tortuga/sqlite_statement.h"
#include "tortuga/rpc_opts.h"
#include "tortuga/tortuga.pb.h"

namespace tortuga {
class ProgressManager : boost::noncopyable {
 public:
  ProgressManager(sqlite3* db, folly::CPUThreadPoolExecutor* exec, RpcOpts rpc_opts);
  ~ProgressManager();

  void HandleFindTask();
  void HandleFindTaskByHandle();

  TaskProgress* FindTaskByHandleInExec(const std::string& handle);

 private:
  TaskProgress* FindTask(const TaskIdentifier& t_id);
  TaskProgress* FindTaskInExec(const TaskIdentifier& t_id);
  TaskProgress* FindTaskByHandle(const std::string& handle);

  TaskProgress* FindTaskByBoundStmtInExec(SqliteStatement* stmt);

  folly::CPUThreadPoolExecutor* exec_{ nullptr };
  sqlite3* db_{ nullptr };
  RpcOpts rpc_opts_;

  SqliteStatement select_task_stmt_;
  SqliteStatement select_task_by_identifier_stmt_;
  SqliteStatement select_worker_id_by_uuid_stmt_;
};
}  // namespace tortuga
