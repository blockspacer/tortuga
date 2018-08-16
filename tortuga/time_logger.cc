#include "tortuga/time_logger.h"

#include <chrono>

#include "glog/logging.h"

namespace tortuga {
TimeLogger::TimeLogger(std::string label) {
  label_.swap(label);
  auto now = std::chrono::system_clock::now().time_since_epoch();
  start_ = std::chrono::duration_cast<std::chrono::milliseconds>(now);
}

TimeLogger::~TimeLogger() {
  auto now = std::chrono::system_clock::now().time_since_epoch();
  auto end = std::chrono::duration_cast<std::chrono::milliseconds>(now);
  VLOG(3) << "Timer labelled: " << label_ << " elapsed ms: " << (end - start_).count();
}
}  // tortuga
