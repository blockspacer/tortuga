#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "boost/utility.hpp"
#include "boost/utility.hpp"

#include "folly/executors/CPUThreadPoolExecutor.h"
#include "sqlite3.h"

#include "tortuga/storage/sqlite_statement.h"
#include "tortuga/time_utils.h"
#include "tortuga/tortuga.pb.h"

namespace tortuga {
struct WorkerTaskInfo {
  int64_t handle{ 0 };
  // when the task was first seen missing.
  // -1 means it was never seen missing.
  int64_t first_miss_millis{ -1 };
};

struct WorkerInfo {
  std::string id;
  std::string uuid;

  int64_t last_beat_millis{ 0 };
  // Tasks that we assigned to a worker.
  // This may be used to check, when a worker heartbeats with some of those missing, they are dead tasks.
  std::map<int64_t, WorkerTaskInfo> tasks;
};

// Must only be called in EXEC
typedef folly::Function<void(const std::string&)> OnWorkerDeath;

class WorkersManager : boost::noncopyable {
 public:
  WorkersManager(sqlite3* db,
                 folly::CPUThreadPoolExecutor* exec,
                 OnWorkerDeath on_worker_death);
  ~WorkersManager();

  // Must be called at startup.
  // Returns a list of tasks to unassign.
  void LoadWorkers();

  void Beat(const HeartbeatReq::WorkerBeat& worker_beat);
  void CheckHeartbeats();
  bool IsKnownWorker(const Worker& worker);

  void OnTaskAssign(int64_t handle,
                    const std::string& worker_id,
                    const std::string& worker_uuid);
  void OnTaskComplete(int64_t handle, const Worker& worker);

 private:
   // unassign all the tasks of this uuid worker.
  void UnassignTasksOfWorkerInExec(const std::string& uuid);
  void UnassignTasksOfWorkersInExec(const std::vector<std::string>& uuids);

  void UnassignTaskInExec(int64_t handle);

  void InsertHistoricWorkerInExec(const std::string& uuid,
                                  const std::string& worker_id);

  void RegularBeat(const HeartbeatReq::WorkerBeat& worker_beat, WorkerInfo* worker_info);
  void WorkerChangeBeat(const Worker& worker, WorkerInfo* worker_info);
  void NewWorkerBeat(const Worker& worker);

  folly::CPUThreadPoolExecutor* exec_{ nullptr };
  sqlite3* db_{ nullptr };
  
  // by id...
  std::map<std::string, WorkerInfo> workers_;

  SqliteStatement select_worker_uuid_stmt_;
  SqliteStatement update_worker_beat_stmt_;
  SqliteStatement update_worker_stmt_;
  SqliteStatement insert_worker_stmt_;
  SqliteStatement insert_historic_worker_stmt_;

  SqliteStatement unassign_tasks_stmt_;
  SqliteStatement unassign_single_task_stmt_;
  SqliteStatement update_worker_invalidated_uuid_stmt_;
  SqliteStatement select_expired_workers_stmt_;

  OnWorkerDeath on_worker_death_;
};
}  // namespace tortuga
