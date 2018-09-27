#pragma once

#include <map>
#include <string>

#include "boost/utility.hpp"

namespace tortuga {
struct WorkerInfo {
  std::string uuid;
  std::string worker_id;
  int64_t last_beat{ 0 };
};

class WorkersCache : boost::noncopyable {
 public:
  WorkersCache();
  ~WorkersCache();

 private:
  std::map<std::string, WorkerInfo> workers_by_id_; 
};
}  // namespace tortuga
