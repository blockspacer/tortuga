#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "boost/utility.hpp"
#include "boost/utility.hpp"

#include "folly/executors/CPUThreadPoolExecutor.h"
#include "sqlite3.h"

#include "tortuga/sqlite_statement.h"
#include "tortuga/time_utils.h"
#include "tortuga/tortuga.pb.h"

namespace tortuga {
struct WorkerInfo {
  std::string id;
  std::string uuid;

  int64_t last_beat_millis{ 0 };
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

  void Beat(const Worker& worker);
  void CheckHeartbeats();
  bool IsKnownWorker(const Worker& worker);

 private:
   // unassign all the tasks of this uuid worker.
  void UnassignTaskInExec(const std::string& uuid);
  void UnassignTasksInExec(const std::vector<std::string>& uuids);

  void InsertHistoricWorkerInExec(const std::string& uuid,
                                  const std::string& worker_id);

  void RegularBeat(const Worker& worker, WorkerInfo* worker_info);
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
  SqliteStatement update_worker_invalidated_uuid_stmt_;
  SqliteStatement select_expired_workers_stmt_;

  OnWorkerDeath on_worker_death_;
};
}  // namespace tortuga
