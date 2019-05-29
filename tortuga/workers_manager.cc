#include "tortuga/workers_manager.h"

#include "folly/Conv.h"
#include "folly/MapUtil.h"
#include "folly/String.h"
#include "folly/fibers/Promise.h"
#include "glog/logging.h"

#include "tortuga/time_logger.h"
#include "tortuga/time_utils.h"

namespace tortuga {
WorkersManager::WorkersManager(std::shared_ptr<TortugaStorage> storage,
                               folly::CPUThreadPoolExecutor* exec, 
                               OnWorkerDeath on_worker_death)
    : storage_(storage),
      exec_(exec),
      on_worker_death_(std::move(on_worker_death)) {
}

WorkersManager::~WorkersManager() {
  exec_ = nullptr;
}

void WorkersManager::LoadWorkers() {
  LOG(INFO) << "rebuilding workers cache from base.";
  std::vector<int64_t> to_unassign;
  TasksWorkedOnIterator it = storage_->IterateTasksWorkedOn();
  
  for (;;) {
    folly::Optional<TaskWorkedOn> task_opt = it.Next();

    if (!task_opt) {
      break;
    }

    const TaskWorkedOn& task = *task_opt;

    folly::Optional<std::string> id_opt = storage_->FindWorkerIdByUuidUnprepared(task.worker_uuid);

    if (!id_opt) {
      LOG(WARNING) << "Task: " << task.row_id << " belonging to unknown worker: " << task.worker_uuid << ", we will unassign";
      to_unassign.push_back(task.row_id);
    } else {
      std::string id = *id_opt;
      WorkerInfo* worker_info = folly::get_ptr(workers_, id);
      if (worker_info == nullptr) {
        WorkerInfo& w = workers_[id];
        w.uuid = task.worker_uuid;
        w.id = id;
        w.last_beat_millis = CurrentTimeMillis();
        worker_info = folly::get_ptr(workers_, id);
      }

      worker_info->tasks[task.row_id].handle = task.row_id;
    }
  }

  LOG(INFO) << "successfully loaded: " << workers_.size() << " workers";
  for (int64_t row_id : to_unassign) {
    storage_->UnassignTaskNotCommit(row_id);
    LOG(INFO) << "unassigned orphan: " << row_id;
  }
}

void WorkersManager::InsertHistoricWorkerInExec(const std::string& uuid,
                                                const std::string& worker_id) {
  storage_->InsertHistoricWorkerNotCommit(uuid, worker_id);
}

void WorkersManager::UnassignTasksOfWorkerInExec(const std::string& uuid) {
  VLOG(2) << "unassigning tasks of expired worker: " << uuid;

  Tx tx(storage_->StartTx());
  storage_->UnassignTasksOfWorkerNotCommit(uuid);
  storage_->InvalidateExpiredWorkerNotCommit(uuid);

  on_worker_death_(uuid);
}

void WorkersManager::UnassignTaskInExec(int64_t handle) {
  storage_->UnassignTaskNotCommit(handle);

  LOG(WARNING) << "reclaimed dead task: " << handle;
}

namespace {
void FindMissingTasks(int64_t expired_tasks_ms,
                      WorkerInfo* worker,
                      std::vector<int64_t>* expired_handles) {
  auto it = worker->tasks.begin();
  while (it != worker->tasks.end()) {
    WorkerTaskInfo& task_info = it->second;
    if (task_info.first_miss_millis != -1 && task_info.first_miss_millis < expired_tasks_ms) {
      expired_handles->push_back(task_info.handle);
      auto toErase = it;
      ++it;
      worker->tasks.erase(toErase);
    } else {
      ++it;
    }
  }
}
}  // anonymous namespace

void WorkersManager::CheckHeartbeats() {
  VLOG(3) << "checking heartbeats";
  int64_t now = CurrentTimeMillis();
  // Workers who haven't beat since then are dead.
  int64_t expired_millis = now - 30000L;
  // Tasks that have been missing since then are dead.
  int64_t expired_tasks_millis = now - 15000L;
  std::vector<int64_t> dead_tasks_handles;
  std::vector<std::string> dead_worker_uuids;

  auto it = workers_.begin();
  while (it != workers_.end()) {
    WorkerInfo& info = it->second;
    if (info.last_beat_millis < expired_millis) {
      dead_worker_uuids.push_back(info.uuid);
      auto toErase = it;
      ++it;
      workers_.erase(toErase);
    } else {
      FindMissingTasks(expired_tasks_millis, &info, &dead_tasks_handles);
       ++it;
    }
  }

  folly::fibers::await([&](folly::fibers::Promise<folly::Unit> p) {
    exec_->add([this, promise = std::move(p), exp_uuids = std::move(dead_worker_uuids), exp_tasks = std::move(dead_tasks_handles)]() mutable {
      UnassignTasksOfWorkersInExec(exp_uuids);
      
      for (int64_t handle : exp_tasks) {
        UnassignTaskInExec(handle);
      }

      promise.setValue(folly::Unit());
    });
  });

  VLOG(3) << "done checking heartbeats...";
}

void WorkersManager::UnassignTasksOfWorkersInExec(const std::vector<std::string>& uuids) {
  for (const auto& uuid : uuids) {
    UnassignTasksOfWorkerInExec(uuid);
  }
}

void WorkersManager::Beat(const HeartbeatReq::WorkerBeat& worker_beat) {
  const Worker& worker = worker_beat.worker();
  WorkerInfo* worker_info = folly::get_ptr(workers_, worker.worker_id());
  if (worker_info != nullptr) {
    if (worker_info->uuid == worker.uuid()) {
      RegularBeat(worker_beat, worker_info);
    } else {
      WorkerChangeBeat(worker, worker_info);
    }
  } else {
    NewWorkerBeat(worker);
  }
}

void WorkersManager::RegularBeat(const HeartbeatReq::WorkerBeat& worker_beat, WorkerInfo* worker_info) {
  const Worker& worker = worker_beat.worker();
  VLOG(3) << "beating worker " << worker.uuid() << " is known and uptodate";
  worker_info->last_beat_millis = CurrentTimeMillis();

  // Check the tasks.
  std::set<int64_t> handles_of_worker;
  for (int64_t h : worker_beat.current_task_handles()) {
    handles_of_worker.insert(h);
  }

  for (auto& it : worker_info->tasks) {
    WorkerTaskInfo& info = it.second;
    bool worker_has_it = handles_of_worker.find(info.handle) != handles_of_worker.end();
    if (!worker_has_it) {
      if (info.first_miss_millis == -1) {
        LOG(ERROR) << "Oops, we have a dead task: " << info.handle;
        info.first_miss_millis = CurrentTimeMillis();
      }
    } else {
      info.first_miss_millis = -1;
    }
  }
}

void WorkersManager::WorkerChangeBeat(const Worker& worker,  WorkerInfo* worker_info) {
  LOG(INFO) << "beating worker " << worker.uuid() << " has changed uuid.";

  folly::fibers::await([&](folly::fibers::Promise<folly::Unit> p) {
    exec_->add([this, promise = std::move(p), old_uuid = worker_info->uuid, &worker]() mutable {
      // invalidate all the tasks assigned to this whole worker.
      UnassignTasksOfWorkerInExec(old_uuid);
      storage_->UpdateNewWorkerNotCommit(worker);
      
      InsertHistoricWorkerInExec(worker.uuid(), worker.worker_id());
      promise.setValue(folly::Unit());
    });
  });

  worker_info->uuid = worker.uuid();
  worker_info->last_beat_millis = CurrentTimeMillis();
  worker_info->tasks.clear();
}

void WorkersManager::NewWorkerBeat(const Worker& worker) {
  LOG(INFO) << "we are welcoming a new worker to the cluster! " << worker.ShortDebugString();

  folly::fibers::await([&](folly::fibers::Promise<folly::Unit> p) {
    exec_->add([this, promise = std::move(p), &worker]() mutable {
      storage_->InsertWorkerNotCommit(worker);

      InsertHistoricWorkerInExec(worker.uuid(), worker.worker_id());
      promise.setValue(folly::Unit());
    });
  });

  WorkerInfo& new_info = workers_[worker.worker_id()];
  new_info.id = worker.worker_id();
  new_info.uuid = worker.uuid();
  new_info.last_beat_millis = CurrentTimeMillis();
}

bool WorkersManager::IsKnownWorker(const Worker& worker) {
  WorkerInfo* worker_info = folly::get_ptr(workers_, worker.worker_id());
  if (worker_info == nullptr) {
    return false;
  }

  return worker_info->uuid == worker.uuid();
}

void WorkersManager::OnTaskAssign(int64_t handle,
                                  const std::string& worker_id,
                                  const std::string& worker_uuid) {
  WorkerInfo* worker_info = folly::get_ptr(workers_, worker_id);
  // This could theorically happen if a heartbeat cleaned up a worker while we are assigning
  // some tasks to it in Exec. However this is highly weird, since a worker that request tasks
  // would most likely be heartbeating normally?
  CHECK(worker_info) << "How could we assign to an unknown worker?";
  CHECK(worker_info->uuid == worker_uuid) << "How could we assign to outdated worker?";

  worker_info->tasks[handle].handle = handle;
}

void WorkersManager::OnTaskComplete(int64_t handle, const Worker& worker) {
  WorkerInfo* worker_info = folly::get_ptr(workers_, worker.worker_id());
  // see above.
  CHECK(worker_info) << "How could we complete from an unknown worker?";
  CHECK(worker_info->uuid == worker.uuid()) << "How could we complete from an outdated worker?";
  worker_info->tasks.erase(handle);
}
}  // namespace tortuga
