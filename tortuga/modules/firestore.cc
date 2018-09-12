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
  std::string url = "https://firestore.googleapis.com/v1beta1/projects/"
      + FLAGS_firestore_project + "/databases/(default)/documents/
      + "" tortugas/32"

  std::map<std::string, std::string> headers;
  headers["Content-Type"] = "application/json";

  http_->Patch();

curl -X PATCH \
     -H "Content-Type: application/json" \
     -d '{"fields" : { "handle" : { "stringValue" : "handle_2" } }}' \
     'https://firestore.googleapis.com/v1beta1/projects/leftyv2-1136/databases/(default)/documents/tortugas/32'
}
}  // namespace tortuga
