#pragma once

#include "tortuga/module.h"

namespace tortuga {
class FirestoreModule : public Module {
 public:
  FirestoreModule();
  ~FirestoreModule() override;

  void OnProgressUpdate() override;
};
}  // namespace tortuga
