#include "tortuga/modules/firestore.h"

#include "folly/dynamic.h"
#include "folly/json.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_string(firestore_project, "", "firestore project to update.");
DEFINE_string(firestore_collection, "", "firestore collection under which to update tasks.");

namespace tortuga {
using folly::dynamic;

FirestoreModule::FirestoreModule(HttpClient* http) : http_(http) {
  CHECK(http != nullptr);
}

FirestoreModule::~FirestoreModule() {
}

void FirestoreModule::OnProgressUpdate(const TaskProgress& task) {
  std::string url = "https://firestore.googleapis.com/v1beta1/projects/"
      + FLAGS_firestore_project
      + "/databases/(default)/documents/"
      + FLAGS_firestore_collection
      + "/"
      + task.handle();

  std::map<std::string, std::string> headers;
  headers["Content-Type"] = "application/json";

  dynamic fields = dynamic::object;
  fields["uuid"] = dynamic::object("stringValue", task.handle());

  // Map<String, Object> token = new HashMap<>();
  // token.put("uuid", id);
  // token.put("done", true);
  // token.put("progress", 1.0d);
  //  token.put("done_date", new Date());
  //  token.put("status_code", status.getCode().value());
  //  if (!Strings.isNullOrEmpty(status.getDescription())) {
  //    token.put("status_description", status.getDescription());
  //  }


  folly::dynamic body = folly::dynamic::object;
  
  

  http_->Patch(url, headers, "");
}
}  // namespace tortuga
