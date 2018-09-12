#pragma once

namespace tortuga {
class Module {
 public:
  Module();
  virtual ~Module();

  virtual void OnProgressUpdate() = 0;
};
}  // namespace tortuga
