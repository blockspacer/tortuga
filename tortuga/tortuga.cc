#include "tortuga/tortuga.h"

#include <chrono>
#include <string>
#include <utility>

#include "folly/Conv.h"
#include "folly/MapUtil.h"
#include "folly/ScopeGuard.h"
#include "folly/String.h"
#include "folly/Unit.h"
#include "glog/logging.h"
#include "grpc++/grpc++.h"
#include "google/protobuf/timestamp.pb.h"
#include "google/protobuf/util/time_util.h"

#include "tortuga/baton_handler.h"
#include "tortuga/time_logger.h"
#include "tortuga/time_utils.h"

using google::protobuf::Timestamp;
using google::protobuf::util::TimeUtil;

namespace tortuga {

TortugaHandler::TortugaHandler(std::shared_ptr<TortugaStorage> storage, RpcOpts rpc_opts, std::map<std::string, std::unique_ptr<Module>> modules)
    : storage_(storage),
      rpc_opts_(rpc_opts),
      modules_(std::move(modules)) {
  progress_mgr_.reset(new ProgressManager(storage, &exec_, rpc_opts));
  workers_manager_.reset(new WorkersManager(storage, &exec_, [this](const std::string& uuid) {
    // cleanup the prepared statement that would otherwise take up memory.
    storage_->Cleanup(uuid);
  }));

  workers_manager_->LoadWorkers();
}

void TortugaHandler::HandleCreateTask() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  CreateReq req;
  grpc::ServerAsyncResponseWriter<CreateResp> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestCreateTask(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleCreateTask();
  });

  VLOG(3) << "received CreateTask RPC: " << req.ShortDebugString();

  CreateTaskResult res = CreateTask(req.task());
  if (res.created) {
    LOG(INFO) << "IN THIS";
    const auto& modules = req.task().modules();
    std::vector<std::string> mods_vec(modules.begin(), modules.end());
    MaybeNotifyModulesOfCreation(res.handle, mods_vec);
  }
    LOG(INFO) << "IN THAT";

  CreateResp reply;
  reply.set_handle(res.handle);
  reply.set_created(res.created);

  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

void TortugaHandler::HandleRequestTask() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  TaskReq req;
  grpc::ServerAsyncResponseWriter<TaskResp> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestRequestTask(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleRequestTask();
  });

  VLOG(3) << "received RequestTask RPC: " << req.ShortDebugString();
  TaskResp reply;

  if (!workers_manager_->IsKnownWorker(req.worker())) {
    // For simplicity reasons, if this is a new worker, we never send it tasks until it has been declared.
    // Hopefully this worker has a functioning Heartbeat() that will declare it real soon.
    reply.set_none(true);
  } else {
    RequestTaskResult res = RequestTask(req.worker(), ctx.deadline(), true);
    if (res.none) {
      reply.set_none(true);
    } else {
      // update firestore to set it worked on.
      MaybeNotifyModulesOfAssignment(res);

      reply.set_id(res.id);
      reply.set_type(res.type);
      CHECK(reply.mutable_data()->ParseFromString(res.data));
      reply.set_handle(folly::to<std::string>(res.handle));
      reply.mutable_retry_ctx()->set_retries(res.retries);
      reply.mutable_retry_ctx()->set_progress_metadata(res.progress_metadata);
      reply.set_priority(res.priority);
    }
  }

  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

TortugaHandler::CreateTaskResult TortugaHandler::CreateTask(const Task& task) {
  auto result = folly::fibers::await([&](folly::fibers::Promise<CreateTaskResult> p) {
    exec_.add([this, &task, promise = std::move(p)]() mutable {
      promise.setValue(CreateTaskInExec(task));
    });
  });

  // See if we can notify someone.
  std::set<folly::fibers::Baton*>& batons = waiting_for_tasks_[task.type()];
  if (!batons.empty()) {
    auto it = batons.begin();
    folly::fibers::Baton* baton = *it;
    batons.erase(it);
    baton->post();
  }

  return result;
}

TortugaHandler::CreateTaskResult TortugaHandler::CreateTaskInExec(const Task& task) {
  TimeLogger create_timer("insert_task");
  Tx tx(storage_->StartTx());

  folly::Optional<int64_t> rowid_opt = storage_->FindTaskById(task.id());
  if (rowid_opt) {
    int64_t rowid = *rowid_opt;
    VLOG(3) << "CreateTask found existing rowid: " << rowid;
    CreateTaskResult res;
    res.handle = folly::to<std::string>(rowid);
    res.created = false;
    
    return res;
  }

  VLOG(3) << "CreateTask: no existing row found for id: " << task.id();
  int64_t rowid = storage_->InsertTaskNotCommit(task);

  CreateTaskResult res;
  res.handle = folly::to<std::string>(rowid);
  res.created = true;

  return res;
}

RequestTaskResult TortugaHandler::RequestTask(const Worker& worker,
                                                              std::chrono::system_clock::time_point rpc_exp,
                                                              bool first_try) {
  RequestTaskResult res = folly::fibers::await([&](folly::fibers::Promise<RequestTaskResult> p) {
    exec_.add([this, &worker, promise = std::move(p)]() mutable {
      promise.setValue(RequestTaskInExec(worker));
    });
  });

  if (!res.none) {
    workers_manager_->OnTaskAssign(res.handle, worker.worker_id(), worker.uuid());
  } else {
    if (first_try) {
      auto now = std::chrono::system_clock::now();
      auto delay = rpc_exp - now;
      if (delay > std::chrono::seconds(1)) {
        // If there is no task, then we can sleep in the hope that there can be one soon.
        // we only do that if there is a second or more left to the RPC deadline.
        VLOG(2) << "worker: " << worker.uuid() << "will be waiting in case a task shows up";
        folly::fibers::Baton baton;
        RegisterWaitingWorker(worker, &baton);
        // we leave a second to answer and end the RPC without deadlock.
        auto wait_duration = delay - std::chrono::seconds(1);
        SCOPE_EXIT {
          UnregisterWaitingWorker(worker, &baton);
        };
        if (baton.try_wait_for(wait_duration)) {
          VLOG(1) << "cool, it was worth waiting, requesting task for: " << worker.uuid();
          return RequestTask(worker, rpc_exp, false);
        }
      }
    }
  }

  return res;
}

RequestTaskResult TortugaHandler::RequestTaskInExec(const Worker& worker) {
  TimeLogger get_task_timer("get_task");
  Tx tx(storage_->StartTx());

  RequestTaskResult res = storage_->RequestTaskNotCommit(worker);
  if (res.none) {
    return res;
  }

  storage_->AssignNotCommit(res.retries, worker.uuid(), res.handle);
  return res;
}

void TortugaHandler::RegisterWaitingWorker(const Worker& worker, folly::fibers::Baton* baton) {
  for (const auto& cap : worker.capabilities()) {
    waiting_for_tasks_[cap].insert(baton);
  }
}

void TortugaHandler::UnregisterWaitingWorker(const Worker& worker, folly::fibers::Baton* baton) {
  for (const auto& cap : worker.capabilities()) {
    waiting_for_tasks_[cap].erase(baton);
  }
}

void TortugaHandler::HandleHeartbeat() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  HeartbeatReq req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestHeartbeat(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleHeartbeat();
  });

  VLOG(3) << "received Heartbeat RPC: " << req.ShortDebugString();
  VLOG(3) << "after this req the fibers allocated is: " << rpc_opts_.fibers->fibersAllocated()
          << " pool size: " << rpc_opts_.fibers->fibersPoolSize();

  for (const auto& worker_beat : req.worker_beats()) {
    workers_manager_->Beat(worker_beat);
  }

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

void TortugaHandler::HandleCompleteTask() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  CompleteTaskReq req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestCompleteTask(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleCompleteTask();
  });

  VLOG(3) << "received CompleteTask RPC: " << req.ShortDebugString();
  std::unique_ptr<UpdatedTask> progress(CompleteTask(req));
  if (progress != nullptr) {
    MaybeNotifyModulesOfUpdate(*progress);
    UpdateProgressManagerCache(*progress);
    workers_manager_->OnTaskComplete(folly::to<int64_t>(progress->progress->handle()),
        req.worker());
  }

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

UpdatedTask* TortugaHandler::CompleteTask(const CompleteTaskReq& req) {
  return folly::fibers::await([&](folly::fibers::Promise<UpdatedTask*> p) {
    exec_.add([this, &req, promise = std::move(p)]() mutable {
      promise.setValue(CompleteTaskInExec(req));
    });
  });
}

UpdatedTask* TortugaHandler::CompleteTaskInExec(const CompleteTaskReq& req) {
  int64_t rowid = folly::to<int64_t>(req.handle());
  VLOG(3) << "completing task of handle: " << rowid;

  folly::Optional<TaskToComplete> task_opt = storage_->SelectTaskToCompleteNotCommit(rowid);

  if (!task_opt) {
    LOG(WARNING) << "completed task doesn't exist! " << req.ShortDebugString();
    return nullptr;
  }

  const std::string& uuid = task_opt->worker_uuid;
  const std::string& worker_uuid = req.worker().uuid();
  if (uuid != worker_uuid) {
    VLOG(1) << "Task doesn't belong to the worker anymore (uuid is: " << uuid << " while worker is: " << worker_uuid << ")";
    return nullptr;
  }

  int max_retries = task_opt->max_retries;
  int retries = task_opt->retries;
  bool ok = req.code() == grpc::StatusCode::OK;
  bool done = ok ? true : (retries >= max_retries);

  storage_->CompleteTaskNotCommit(rowid, req, done);

  return progress_mgr_->FindTaskByHandleInExec(rowid);
}

void TortugaHandler::CheckHeartbeatsLoop() {
  // This is how we do repeated TimedTasks in a fiber since nobody
  // will post to the Baton we'll just sleep/execute repeatedly.
  for (;;) {
    folly::fibers::Baton baton;
    CHECK(!baton.timed_wait(std::chrono::milliseconds(500)));
    workers_manager_->CheckHeartbeats();
  }
}

void TortugaHandler::HandleUpdateProgress() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  UpdateProgressReq req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestUpdateProgress(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandleUpdateProgress();
  });

  VLOG(3) << "received HandleUpdateProgress RPC: " << req.ShortDebugString();
  std::unique_ptr<UpdatedTask> progress(UpdateProgress(req));
  if (progress != nullptr) {
    MaybeNotifyModulesOfUpdate(*progress);
    UpdateProgressManagerCache(*progress);
  }

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

UpdatedTask* TortugaHandler::UpdateProgress(const UpdateProgressReq& req) {
  return folly::fibers::await([&](folly::fibers::Promise<UpdatedTask*> p) {
    exec_.add([this, &req, promise = std::move(p)]() mutable {
      promise.setValue(UpdateProgressInExec(req));
    });
  });
}

UpdatedTask* TortugaHandler::UpdateProgressInExec(const UpdateProgressReq& req) {
  int64_t rowid = folly::to<int64_t>(req.handle());
  VLOG(3) << "updating task of handle: " << rowid;
  folly::Optional<TaskToComplete> task_opt = storage_->SelectTaskToCompleteNotCommit(rowid);

  if (!task_opt) {
    LOG(WARNING) << "updating task doesn't exist! " << req.ShortDebugString();
    return nullptr;
  }

  const auto& task = *task_opt;

  const std::string& worker_uuid = req.worker().uuid();
  if (task.worker_uuid != worker_uuid) {
    VLOG(1) << "Task doesn't belong to the worker anymore (uuid is: " << task.worker_uuid << " while worker is: " << worker_uuid << ")";
    return nullptr;
  }

  storage_->UpdateProgressNotCommit(rowid, req);

  return progress_mgr_->FindTaskByHandleInExec(rowid);
}

void TortugaHandler::MaybeNotifyModulesOfUpdate(const UpdatedTask& task) {
  for (const auto& module_name : task.modules) {
    const std::unique_ptr<Module>* module = folly::get_ptr(modules_, module_name);
    if (module != nullptr) {
      VLOG(2) << "notifying module: " << module_name << " of task progres: " << task.progress->id();
      (*module)->OnProgressUpdate(*task.progress);
    }
  }
}

void TortugaHandler::MaybeNotifyModulesOfCreation(const std::string& handle,
                                                  const std::vector<std::string> modules) {
  TaskProgress progress;
  progress.set_handle(handle);
  *progress.mutable_created() = TimeUtil::GetCurrentTime();
  // any remaining fields as defaults is fine (worked_on = false etc...).

  for (const auto& module_name : modules) {
    const std::unique_ptr<Module>* module = folly::get_ptr(modules_, module_name);
    if (module != nullptr) {
      VLOG(2) << "notifying module: " << module_name << " of task creation: " << progress.id();
      (*module)->OnProgressUpdate(progress);
    }
  }
}

void TortugaHandler::MaybeNotifyModulesOfAssignment(const RequestTaskResult& res) {
  TaskProgress progress;
  progress.set_handle(folly::to<std::string>(res.handle));
  progress.set_worked_on(true);
  progress.set_retries(res.retries);
  progress.set_done(false);

  for (const auto& module_name : res.modules) {
    const std::unique_ptr<Module>* module = folly::get_ptr(modules_, module_name);
    if (module != nullptr) {
      VLOG(2) << "notifying module: " << module_name << " of task assignment: " << progress.id();
      (*module)->OnProgressUpdate(progress);
    }
  }
}

void TortugaHandler::UpdateProgressManagerCache(const UpdatedTask& task) {
  progress_mgr_->UpdateTaskProgressCache(*task.progress);
}

void TortugaHandler::HandlePing() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  google::protobuf::Empty req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestPing(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  // adds a new RPC processor.
  rpc_opts_.fibers->addTask([this]() {
    HandlePing();
  });

  VLOG(5) << "received Ping RPC: " << req.ShortDebugString();

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
}

void TortugaHandler::HandleQuit() {
  BatonHandler handler;

  grpc::ServerContext ctx;
  google::protobuf::Empty req;
  grpc::ServerAsyncResponseWriter<google::protobuf::Empty> resp(&ctx);

  // start a new RPC and wait.
  rpc_opts_.tortuga_grpc->RequestQuitQuitQuit(&ctx, &req, &resp, rpc_opts_.cq, rpc_opts_.cq, &handler);
  CHECK(handler.Wait());

  google::protobuf::Empty reply;
  handler.Reset();
  resp.Finish(reply, grpc::Status::OK, &handler);
  handler.Wait();
  LOG(FATAL) << "received QuitQuitQuit command, there is no coming back";
}
}  // namespace tortuga
