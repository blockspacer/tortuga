#include "tortuga/progress_manager.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "folly/Conv.h"
#include "folly/MapUtil.h"
#include "folly/String.h"
#include "grpc++/grpc++.h"

#include "tortuga/baton_handler.h"
#include "tortuga/time_logger.h"
#include "tortuga/tortuga.grpc.pb.h"

namespace tortuga {
ProgressManager::ProgressManager(std::shared_ptr<TortugaStorage> storage,
                                 folly::CPUThreadPoolExecutor* exec,
                                 RpcOpts rpc_opts)
    : storage_(storage),
      db_(storage->db()),
      exec_(exec),
      rpc_opts_(rpc_opts) {  
}

ProgressManager::~ProgressManager() {
}

void ProgressManager::HandleFindTask() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  TaskIdentifier req;
  grpc::ServerAsyncResponseWriter<TaskProgress> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestFindTask(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleFindTask();
  });

  VLOG(3) << "received FindTask RPC: " << req.ShortDebugString();

  std::unique_ptr<UpdatedTask> progress(FindTask(req));
  handler.Reset();
  if (progress == nullptr) {
    TaskProgress unfound;
    auto status_not_found = grpc::Status(grpc::StatusCode::NOT_FOUND, "no such task");
    resp.Finish(unfound, status_not_found, &handler);
  } else {
    resp.Finish(*progress->progress, grpc::Status::OK, &handler);
  }

  handler.Wait();
}

void ProgressManager::HandleFindTaskByHandle() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  FindTaskReq req;
  grpc::ServerAsyncResponseWriter<TaskProgress> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestFindTaskByHandle(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  TimeLogger time_log("find_task_by_handle");

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleFindTaskByHandle();
  });

  VLOG(3) << "received FindTaskByHandle RPC: " << req.ShortDebugString();

  auto it = progress_cache_.find(req.handle());
  if (it != progress_cache_.end()) {
    VLOG(3) << "progress hit from the cache :)";
    // we must copy otherwise the cache could evict the object.
    TaskProgress resp_obj = it->second;
    handler.Reset();
    resp.Finish(resp_obj, grpc::Status::OK, &handler);
    handler.Wait();
    return;
  }

  std::unique_ptr<UpdatedTask> progress(FindTaskByHandle(req));

  handler.Reset();
  if (progress == nullptr) {
    TaskProgress unfound;
    auto status_not_found = grpc::Status(grpc::StatusCode::NOT_FOUND, "no such task");
    resp.Finish(unfound, status_not_found, &handler);
  } else {
    resp.Finish(*progress->progress, grpc::Status::OK, &handler);
  }

  handler.Wait();
}

UpdatedTask* ProgressManager::FindTaskByHandle(const FindTaskReq& req) {
  folly::fibers::await([&](folly::fibers::Promise<UpdatedTask*> p) {
    exec_->add([this, &req, promise = std::move(p)]() mutable {
      promise.setValue(FindTaskByHandleInExec(req.handle()));
    });
  });
}

UpdatedTask* ProgressManager::FindTaskByHandleInExec(int64_t handle) {
  return storage_->FindUpdatedTaskByHandle(handle);
}

UpdatedTask* ProgressManager::FindTask(const TaskIdentifier& t_id) {
  folly::fibers::await([&](folly::fibers::Promise<UpdatedTask*> p) {
    exec_->add([this, &t_id, promise = std::move(p)]() mutable {
      promise.setValue(FindTaskInExec(t_id));
    });
  });
}

UpdatedTask* ProgressManager::FindTaskInExec(const TaskIdentifier& t_id) {
  return storage_->FindUpdatedTask(t_id);
}

void ProgressManager::UpdateTaskProgressCache(const TaskProgress& progress) {
  progress_cache_.set(folly::to<int64_t>(progress.handle()), progress);
}
}  // namespace tortuga
