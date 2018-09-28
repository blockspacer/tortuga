#pragma once

#include <map>
#include <string>

#include "boost/utility.hpp"

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
  WorkersCache();
  ~WorkersCache();

  void Beat(const Worker& worker);

 private:
  
  int64_t startup_time_{ CurrentTimeMillis() };
  // by id...
  std::map<std::string, WorkerInfo> workers_;
  bool first_check_{ true };
};
}  // namespace tortuga
