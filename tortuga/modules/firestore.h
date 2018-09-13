#pragma once

#include <memory>

#include "google/firestore/v1beta1/firestore.pb.h"
#include "google/firestore/v1beta1/firestore.grpc.pb.h"
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
  std::unique_ptr<google::firestore::v1beta1::Firestore::Stub> stub_;
};
}  // namespace tortuga
