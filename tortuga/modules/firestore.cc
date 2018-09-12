#include "tortuga/modules/firestore.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_string(firestore_project, "", "firestore project to update.");
DEFINE_string(firestore_collection, "", "firestore collection under which to update tasks.");

namespace tortuga {
FirestoreModule::FirestoreModule(HttpClient* http) : http_(http) {
  CHECK(http != nullptr);
}

FirestoreModule::~FirestoreModule() {
}

void FirestoreModule::OnProgressUpdate(const TaskProgress& task) {

}
}  // namespace tortuga
