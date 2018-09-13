#pragma once

#include "grpc++/grpc++.h"

#include "tortuga/module.h"
#include "tortuga/tortuga.pb.h"

namespace tortuga {
class FirestoreModule : public Module {
 public:
  // 'queue' not owned.
  explicit FirestoreModule(grpc::CompletionQueue* queue);
  ~FirestoreModule() override;

  void OnProgressUpdate(const TaskProgress& task) override;

 private:
  grpc::CompletionQueue* queue_{ nullptr };
};
}  // namespace tortuga
