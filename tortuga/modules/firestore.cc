#include "tortuga/modules/firestore.h"

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "google/firestore/v1beta1/document.pb.h"
#include "google/firestore/v1beta1/firestore.pb.h"
#include "google/firestore/v1beta1/firestore.grpc.pb.h"
#include "grpc++/grpc++.h"

#include "tortuga/baton_handler.h"
#include "tortuga/time_utils.h"

DEFINE_string(firestore_project, "leftyv2-1136", "firestore project to update.");
DEFINE_string(firestore_collection, "tortugas", "firestore collection under which to update tasks.");

namespace tortuga {
using google::firestore::v1beta1::Document;
using google::firestore::v1beta1::Firestore;
using google::firestore::v1beta1::UpdateDocumentRequest;
using google::firestore::v1beta1::Value;

FirestoreModule::FirestoreModule(grpc::CompletionQueue* queue) : queue_(queue) {
  CHECK(queue_ != nullptr);
  
  auto chan = grpc::CreateChannel("firestore.googleapis.com:443", grpc::SslCredentials(grpc::SslCredentialsOptions()));
  stub_ = Firestore::NewStub(chan);
}

FirestoreModule::~FirestoreModule() {
}

void FirestoreModule::OnProgressUpdate(const TaskProgress& task) {
  UpdateDocumentRequest req;
  req.mutable_document()->set_name("projects/" + FLAGS_firestore_project + "/databases/(default)/documents/"
      + FLAGS_firestore_collection + "/" + task.handle());
  google::protobuf::Map<std::string, Value>* fields = req.mutable_document()->mutable_fields();
  
  {
    Value handle_value;
    handle_value.set_string_value(task.handle());
    (*fields)["handle"] = handle_value;
  }

  {
    Value done_value;
    done_value.set_boolean_value(task.done());
    (*fields)["done"] = done_value;
  }

  {
    Value progress_value;
    progress_value.set_double_value(static_cast<double>(task.progress()));
    (*fields)["progress"] = progress_value;
  }

  {
    Value progress_msg_value;
    progress_msg_value.set_string_value(task.progress_message());
    (*fields)["progress_msg"] = progress_msg_value;
  }

  {
    Value progress_metadata_value;
    progress_metadata_value.set_string_value(task.progress_metadata());
    (*fields)["progress_metadata"] = progress_metadata_value;
  }

  {
    Value output_value;
    output_value.set_string_value(task.output());
    (*fields)["output"] = output_value;
  }

  if (task.has_status()) {
    {
      Value status_value;
      status_value.set_integer_value(task.status().code());
      (*fields)["status"] = status_value;
    }

    {
      Value status_desc_value;
      status_desc_value.set_string_value(task.status().message());
      (*fields)["status_description"] = status_desc_value;
    }
  }

  grpc::ClientContext ctx;
  SetDeadlineMillis(30000, &ctx);

  std::unique_ptr<grpc::ClientAsyncResponseReader<Document>> rpc(
        stub_->AsyncUpdateDocument(&ctx, req, queue_));
  CHECK(rpc != nullptr);

  Document resp;
  BatonHandler handler;
  grpc::Status status;
  rpc->Finish(&resp, &status, &handler);
  CHECK(handler.Wait());
  CHECK(status.ok()) << status.error_code() << " msg: " << status.error_message();
}
}  // namespace tortuga
