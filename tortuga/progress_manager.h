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
typedef grpc::ServerAsyncReaderWriter<SubResp, SubReq> ProgressStream;

struct SubCtx {
  grpc::ServerContext ctx;
  ProgressStream stream{ &ctx };
};

struct Subscription {
  std::shared_ptr<SubCtx> ctx;
  std::set<int64_t> handles;
};

struct Subscriptions {
  // objects not owned :)
  std::vector<Subscription*> subscriptions;
};

class ProgressManager : boost::noncopyable {
 public:
  ProgressManager(sqlite3* db, folly::CPUThreadPoolExecutor* exec, RpcOpts rpc_opts);
  ~ProgressManager();

  void HandleFindTask();
  void HandleFindTaskByHandle();
  void HandleProgressSubscribe();
  void NotifyProgress(const TaskProgress& task);

  TaskProgress* FindTaskByHandleInExec(const std::string& handle);

 private:
  TaskProgress* FindTask(const TaskIdentifier& t_id);
  TaskProgress* FindTaskInExec(const TaskIdentifier& t_id);
  TaskProgress* FindTaskByHandle(const std::string& handle);

  TaskProgress* FindTaskByBoundStmtInExec(SqliteStatement* stmt);

  folly::CPUThreadPoolExecutor* exec_{ nullptr };
  sqlite3* db_{ nullptr };
  RpcOpts rpc_opts_;

  // From row id to a task.
  std::map<int64_t, Subscriptions> subs_;

  SqliteStatement select_task_stmt_;
  SqliteStatement select_task_by_identifier_stmt_;
  SqliteStatement select_worker_id_by_uuid_stmt_;
};
}  // namespace tortuga
