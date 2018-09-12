#pragma once

#include "tortuga/http.h"
#include "tortuga/module.h"
#include "tortuga/tortuga.pb.h"

namespace tortuga {
class FirestoreModule : public Module {
 public:
  // 'client' not owned.
  explicit FirestoreModule(HttpClient* http);
  ~FirestoreModule() override;

  void OnProgressUpdate(const TaskProgress& task) override;

 private:
  HttpClient* http_{ nullptr };
};
}  // namespace tortuga
