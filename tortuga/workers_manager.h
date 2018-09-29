#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include "boost/utility.hpp"
#include "boost/utility.hpp"

#include "folly/executors/CPUThreadPoolExecutor.h"
#include "sqlite3.h"

#include "tortuga/time_utils.h"
#include "tortuga/tortuga.pb.h"

namespace tortuga {
struct WorkerInfo {
  std::string id;
  std::string uuid;

  int64_t last_beat_millis{ 0 };
};

class WorkersManager : boost::noncopyable {
 public:
  WorkersManager(sqlite3* db, folly::CPUThreadPoolExecutor* exec);
  ~WorkersManager();

  // Must be called at startup.
  // Returns a list of tasks to unassign.
  void LoadWorkers();

  // void Beat(const Worker& worker);

 private:
  // void RegularBeat(const Worker& worker, WorkerInfo* worker_info);
  // void WorkerChangeBeat(const Worker& worker, WorkerInfo* worker_info);
  // void NewWorkerBeat(const Worker& worker);

  folly::CPUThreadPoolExecutor* exec_{ nullptr };
  sqlite3* db_{ nullptr };
  
  int64_t startup_time_{ CurrentTimeMillis() };
  // by id...
  std::map<std::string, WorkerInfo> workers_;
  bool first_check_{ true };
};
}  // namespace tortuga
