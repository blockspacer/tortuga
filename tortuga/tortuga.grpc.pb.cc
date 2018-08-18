// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: tortuga/tortuga.proto

#include "tortuga/tortuga.pb.h"
#include "tortuga/tortuga.grpc.pb.h"

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace tortuga {

static const char* Tortuga_method_names[] = {
  "/tortuga.Tortuga/CreateTask",
  "/tortuga.Tortuga/RequestTask",
  "/tortuga.Tortuga/Heartbeat",
  "/tortuga.Tortuga/CompleteTask",
  "/tortuga.Tortuga/FindTask",
  "/tortuga.Tortuga/FindTaskByHandle",
  "/tortuga.Tortuga/ProgressSubscribe",
  "/tortuga.Tortuga/Ping",
  "/tortuga.Tortuga/QuitQuitQuit",
};

std::unique_ptr< Tortuga::Stub> Tortuga::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Tortuga::Stub> stub(new Tortuga::Stub(channel));
  return stub;
}

Tortuga::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_CreateTask_(Tortuga_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_RequestTask_(Tortuga_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Heartbeat_(Tortuga_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_CompleteTask_(Tortuga_method_names[3], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_FindTask_(Tortuga_method_names[4], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_FindTaskByHandle_(Tortuga_method_names[5], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_ProgressSubscribe_(Tortuga_method_names[6], ::grpc::internal::RpcMethod::BIDI_STREAMING, channel)
  , rpcmethod_Ping_(Tortuga_method_names[7], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_QuitQuitQuit_(Tortuga_method_names[8], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Tortuga::Stub::CreateTask(::grpc::ClientContext* context, const ::tortuga::CreateReq& request, ::tortuga::CreateResp* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_CreateTask_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::tortuga::CreateResp>* Tortuga::Stub::AsyncCreateTaskRaw(::grpc::ClientContext* context, const ::tortuga::CreateReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::tortuga::CreateResp>::Create(channel_.get(), cq, rpcmethod_CreateTask_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::tortuga::CreateResp>* Tortuga::Stub::PrepareAsyncCreateTaskRaw(::grpc::ClientContext* context, const ::tortuga::CreateReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::tortuga::CreateResp>::Create(channel_.get(), cq, rpcmethod_CreateTask_, context, request, false);
}

::grpc::Status Tortuga::Stub::RequestTask(::grpc::ClientContext* context, const ::tortuga::TaskReq& request, ::tortuga::TaskResp* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_RequestTask_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::tortuga::TaskResp>* Tortuga::Stub::AsyncRequestTaskRaw(::grpc::ClientContext* context, const ::tortuga::TaskReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::tortuga::TaskResp>::Create(channel_.get(), cq, rpcmethod_RequestTask_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::tortuga::TaskResp>* Tortuga::Stub::PrepareAsyncRequestTaskRaw(::grpc::ClientContext* context, const ::tortuga::TaskReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::tortuga::TaskResp>::Create(channel_.get(), cq, rpcmethod_RequestTask_, context, request, false);
}

::grpc::Status Tortuga::Stub::Heartbeat(::grpc::ClientContext* context, const ::tortuga::Worker& request, ::google::protobuf::Empty* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Heartbeat_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::google::protobuf::Empty>* Tortuga::Stub::AsyncHeartbeatRaw(::grpc::ClientContext* context, const ::tortuga::Worker& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::google::protobuf::Empty>::Create(channel_.get(), cq, rpcmethod_Heartbeat_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::google::protobuf::Empty>* Tortuga::Stub::PrepareAsyncHeartbeatRaw(::grpc::ClientContext* context, const ::tortuga::Worker& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::google::protobuf::Empty>::Create(channel_.get(), cq, rpcmethod_Heartbeat_, context, request, false);
}

::grpc::Status Tortuga::Stub::CompleteTask(::grpc::ClientContext* context, const ::tortuga::CompleteTaskReq& request, ::google::protobuf::Empty* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_CompleteTask_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::google::protobuf::Empty>* Tortuga::Stub::AsyncCompleteTaskRaw(::grpc::ClientContext* context, const ::tortuga::CompleteTaskReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::google::protobuf::Empty>::Create(channel_.get(), cq, rpcmethod_CompleteTask_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::google::protobuf::Empty>* Tortuga::Stub::PrepareAsyncCompleteTaskRaw(::grpc::ClientContext* context, const ::tortuga::CompleteTaskReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::google::protobuf::Empty>::Create(channel_.get(), cq, rpcmethod_CompleteTask_, context, request, false);
}

::grpc::Status Tortuga::Stub::FindTask(::grpc::ClientContext* context, const ::tortuga::TaskIdentifier& request, ::tortuga::TaskProgress* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_FindTask_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::tortuga::TaskProgress>* Tortuga::Stub::AsyncFindTaskRaw(::grpc::ClientContext* context, const ::tortuga::TaskIdentifier& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::tortuga::TaskProgress>::Create(channel_.get(), cq, rpcmethod_FindTask_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::tortuga::TaskProgress>* Tortuga::Stub::PrepareAsyncFindTaskRaw(::grpc::ClientContext* context, const ::tortuga::TaskIdentifier& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::tortuga::TaskProgress>::Create(channel_.get(), cq, rpcmethod_FindTask_, context, request, false);
}

::grpc::Status Tortuga::Stub::FindTaskByHandle(::grpc::ClientContext* context, const ::google::protobuf::StringValue& request, ::tortuga::TaskProgress* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_FindTaskByHandle_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::tortuga::TaskProgress>* Tortuga::Stub::AsyncFindTaskByHandleRaw(::grpc::ClientContext* context, const ::google::protobuf::StringValue& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::tortuga::TaskProgress>::Create(channel_.get(), cq, rpcmethod_FindTaskByHandle_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::tortuga::TaskProgress>* Tortuga::Stub::PrepareAsyncFindTaskByHandleRaw(::grpc::ClientContext* context, const ::google::protobuf::StringValue& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::tortuga::TaskProgress>::Create(channel_.get(), cq, rpcmethod_FindTaskByHandle_, context, request, false);
}

::grpc::ClientReaderWriter< ::tortuga::SubReq, ::tortuga::SubResp>* Tortuga::Stub::ProgressSubscribeRaw(::grpc::ClientContext* context) {
  return ::grpc::internal::ClientReaderWriterFactory< ::tortuga::SubReq, ::tortuga::SubResp>::Create(channel_.get(), rpcmethod_ProgressSubscribe_, context);
}

::grpc::ClientAsyncReaderWriter< ::tortuga::SubReq, ::tortuga::SubResp>* Tortuga::Stub::AsyncProgressSubscribeRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::tortuga::SubReq, ::tortuga::SubResp>::Create(channel_.get(), cq, rpcmethod_ProgressSubscribe_, context, true, tag);
}

::grpc::ClientAsyncReaderWriter< ::tortuga::SubReq, ::tortuga::SubResp>* Tortuga::Stub::PrepareAsyncProgressSubscribeRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::tortuga::SubReq, ::tortuga::SubResp>::Create(channel_.get(), cq, rpcmethod_ProgressSubscribe_, context, false, nullptr);
}

::grpc::Status Tortuga::Stub::Ping(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::google::protobuf::Empty* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Ping_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::google::protobuf::Empty>* Tortuga::Stub::AsyncPingRaw(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::google::protobuf::Empty>::Create(channel_.get(), cq, rpcmethod_Ping_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::google::protobuf::Empty>* Tortuga::Stub::PrepareAsyncPingRaw(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::google::protobuf::Empty>::Create(channel_.get(), cq, rpcmethod_Ping_, context, request, false);
}

::grpc::Status Tortuga::Stub::QuitQuitQuit(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::google::protobuf::Empty* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_QuitQuitQuit_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::google::protobuf::Empty>* Tortuga::Stub::AsyncQuitQuitQuitRaw(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::google::protobuf::Empty>::Create(channel_.get(), cq, rpcmethod_QuitQuitQuit_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::google::protobuf::Empty>* Tortuga::Stub::PrepareAsyncQuitQuitQuitRaw(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::google::protobuf::Empty>::Create(channel_.get(), cq, rpcmethod_QuitQuitQuit_, context, request, false);
}

Tortuga::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Tortuga_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Tortuga::Service, ::tortuga::CreateReq, ::tortuga::CreateResp>(
          std::mem_fn(&Tortuga::Service::CreateTask), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Tortuga_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Tortuga::Service, ::tortuga::TaskReq, ::tortuga::TaskResp>(
          std::mem_fn(&Tortuga::Service::RequestTask), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Tortuga_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Tortuga::Service, ::tortuga::Worker, ::google::protobuf::Empty>(
          std::mem_fn(&Tortuga::Service::Heartbeat), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Tortuga_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Tortuga::Service, ::tortuga::CompleteTaskReq, ::google::protobuf::Empty>(
          std::mem_fn(&Tortuga::Service::CompleteTask), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Tortuga_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Tortuga::Service, ::tortuga::TaskIdentifier, ::tortuga::TaskProgress>(
          std::mem_fn(&Tortuga::Service::FindTask), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Tortuga_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Tortuga::Service, ::google::protobuf::StringValue, ::tortuga::TaskProgress>(
          std::mem_fn(&Tortuga::Service::FindTaskByHandle), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Tortuga_method_names[6],
      ::grpc::internal::RpcMethod::BIDI_STREAMING,
      new ::grpc::internal::BidiStreamingHandler< Tortuga::Service, ::tortuga::SubReq, ::tortuga::SubResp>(
          std::mem_fn(&Tortuga::Service::ProgressSubscribe), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Tortuga_method_names[7],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Tortuga::Service, ::google::protobuf::Empty, ::google::protobuf::Empty>(
          std::mem_fn(&Tortuga::Service::Ping), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Tortuga_method_names[8],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Tortuga::Service, ::google::protobuf::Empty, ::google::protobuf::Empty>(
          std::mem_fn(&Tortuga::Service::QuitQuitQuit), this)));
}

Tortuga::Service::~Service() {
}

::grpc::Status Tortuga::Service::CreateTask(::grpc::ServerContext* context, const ::tortuga::CreateReq* request, ::tortuga::CreateResp* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Tortuga::Service::RequestTask(::grpc::ServerContext* context, const ::tortuga::TaskReq* request, ::tortuga::TaskResp* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Tortuga::Service::Heartbeat(::grpc::ServerContext* context, const ::tortuga::Worker* request, ::google::protobuf::Empty* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Tortuga::Service::CompleteTask(::grpc::ServerContext* context, const ::tortuga::CompleteTaskReq* request, ::google::protobuf::Empty* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Tortuga::Service::FindTask(::grpc::ServerContext* context, const ::tortuga::TaskIdentifier* request, ::tortuga::TaskProgress* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Tortuga::Service::FindTaskByHandle(::grpc::ServerContext* context, const ::google::protobuf::StringValue* request, ::tortuga::TaskProgress* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Tortuga::Service::ProgressSubscribe(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::tortuga::SubResp, ::tortuga::SubReq>* stream) {
  (void) context;
  (void) stream;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Tortuga::Service::Ping(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::google::protobuf::Empty* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Tortuga::Service::QuitQuitQuit(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::google::protobuf::Empty* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace tortuga

