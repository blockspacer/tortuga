#pragma once

#include <chrono>
#include <string>

namespace tortuga {

class TimeLogger {
 public:
  explicit TimeLogger(std::string label);
  ~TimeLogger();

 private:
  std::string label_;
  std::chrono::milliseconds start_;
};
}  // tortuga
