#pragma once

#include <map>
#include <vector>

#include "boost/utility.hpp"
#include "folly/container/EvictingCacheMap.h"
#include "folly/executors/CPUThreadPoolExecutor.h"
#include "grpc++/grpc++.h"

#include "tortuga/storage/tortuga_storage.h"
#include "tortuga/rpc_opts.h"
#include "tortuga/tortuga.pb.h"
#include "tortuga/updated_task.h"

namespace tortuga {


class ProgressManager : boost::noncopyable {
 public:
  ProgressManager(std::shared_ptr<TortugaStorage> storage, folly::CPUThreadPoolExecutor* exec, RpcOpts rpc_opts);
  ~ProgressManager();

  void HandleFindTask();
  void HandleFindTaskByHandle();

  UpdatedTask* FindTaskByHandleInExec(int64_t handle);
  void UpdateTaskProgressCache(const TaskProgress& progress);

 private:
  UpdatedTask* FindTask(const TaskIdentifier& t_id);
  UpdatedTask* FindTaskInExec(const TaskIdentifier& t_id);
  UpdatedTask* FindTaskByHandle(const FindTaskReq& req);

  UpdatedTask* FindTaskByBoundStmtInExec(SqliteStatement* stmt);

  folly::CPUThreadPoolExecutor* exec_{ nullptr };
  RpcOpts rpc_opts_;

  std::shared_ptr<TortugaStorage> storage_;

  folly::EvictingCacheMap<int64_t, TaskProgress> progress_cache_{ 8192, 128 };
};
}  // namespace tortuga
