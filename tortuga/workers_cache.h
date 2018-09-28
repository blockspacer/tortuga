#pragma once

#include <map>
#include <string>

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

class WorkersCache : boost::noncopyable {
 public:
  WorkersCache(sqlite3* db, folly::CPUThreadPoolExecutor* exec);
  ~WorkersCache();

  void Beat(const Worker& worker);

 private:
  folly::CPUThreadPoolExecutor* exec_{ nullptr };
  sqlite3* db_{ nullptr };
  
  int64_t startup_time_{ CurrentTimeMillis() };
  // by id...
  std::map<std::string, WorkerInfo> workers_;
  bool first_check_{ true };
};
}  // namespace tortuga
