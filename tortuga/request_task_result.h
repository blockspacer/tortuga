#pragma once

#include <string>
#include <vector>

namespace tortuga {
struct RequestTaskResult {
  bool none { false };
  std::string id;
  int64_t handle{ 0 };
  std::string type;
  std::string data;
  int priority{ 0 };
  int retries{ 0 };
  std::string progress_metadata;
  std::vector<std::string> modules;
};
}  // tortuga
