#pragma once

#include "tortuga/tortuga.pb.h"

namespace tortuga {
class Module {
 public:
  Module();
  virtual ~Module();

  virtual void OnProgressUpdate(const TaskProgress& task) = 0;
};
}  // namespace tortuga
