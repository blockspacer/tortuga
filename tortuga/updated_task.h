#pragma once

#include <memory>
#include <vector>

#include "tortuga/tortuga.pb.h"

namespace tortuga {
struct UpdatedTask {
  std::unique_ptr<TaskProgress> progress;
  std::vector<std::string> modules;
};
}  // tortuga
