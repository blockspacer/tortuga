#include <map>
#include <memory>
#include <string>
#include <thread>
#include <utility>

#include "folly/Conv.h"
#include "folly/fibers/FiberManager.h"
#include "folly/fibers/FiberManagerMap.h"
#include "folly/io/async/EventBase.h"
#include "folly/init/Init.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "grpc++/grpc++.h"

#include "tortuga/baton_handler.h"
#include "tortuga/module.h"
#include "tortuga/rpc_opts.h"
#include "tortuga/tortuga.h"
#include "tortuga/tortuga.grpc.pb.h"
#include "tortuga/tortuga.pb.h"
#include "tortuga/modules/firestore.h"
#include "tortuga/storage/tortuga_storage.h"

DEFINE_string(addr, "127.0.0.1", "address to listen on.");
DEFINE_int32(port, 4000, "port to listen on");

void LoopGrpc(grpc::ServerCompletionQueue* cq) {
  while (true) {
    bool ok = true;
    void* tag = nullptr;

    CHECK(cq->Next(&tag, &ok));
    CHECK(tag != nullptr);
    static_cast<tortuga::BatonHandler*>(tag)->Handle(ok);
  }
}

int main(int argc, char** argv) {
  folly::init(&argc, &argv, true);

  auto storage = tortuga::TortugaStorage::Init();

  tortuga::Tortuga::AsyncService tortuga_grpc;
  
  std::string addr = FLAGS_addr + ":" + folly::to<std::string>(FLAGS_port);
  grpc::ServerBuilder builder;
  builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
  builder.RegisterService(&tortuga_grpc);
  std::unique_ptr<grpc::ServerCompletionQueue> cq = builder.AddCompletionQueue();
  std::unique_ptr<grpc::Server> server = builder.BuildAndStart();

  std::thread grpc_thread([&cq]() {
    LoopGrpc(cq.get());
  });
  
  folly::EventBase evb;
  folly::fibers::FiberManager::Options fiber_opts;
  fiber_opts.stackSize = 1024 * 1024;
  folly::fibers::FiberManager& fibers = folly::fibers::getFiberManager(evb, fiber_opts);
  
  tortuga::RpcOpts rpc_opts;
  rpc_opts.tortuga_grpc = &tortuga_grpc;
  rpc_opts.cq = cq.get();
  rpc_opts.fibers = &fibers;

  std::map<std::string, std::unique_ptr<tortuga::Module>> modules;
  modules["firestore"] = std::make_unique<tortuga::FirestoreModule>(cq.get());

  tortuga::TortugaHandler tortuga(storage->db(), rpc_opts, std::move(modules));

  fibers.addTask([&tortuga]() {
    tortuga.HandleCreateTask();
  });

  fibers.addTask([&tortuga]() {
    tortuga.HandleRequestTask();
  });

  fibers.addTask([&tortuga]() {
    tortuga.HandleHeartbeat();
  });

  fibers.addTask([&tortuga]() {
    tortuga.HandleUpdateProgress();
  });

  fibers.addTask([&tortuga]() {
    tortuga.HandleCompleteTask();
  });

  fibers.addTask([&tortuga]() {
    tortuga.HandlePing();
  });

  fibers.addTask([&tortuga]() {
    tortuga.HandleQuit();
  });

  fibers.addTask([&tortuga]() {
    tortuga.CheckHeartbeatsLoop();
  });

  fibers.addTask([&tortuga]() {
    tortuga.HandleFindTask();
  });

  fibers.addTask([&tortuga]() {
    tortuga.HandleFindTaskByHandle();
  });

  LOG(INFO) << "Tortuga is taking the stage on: " << addr;
  evb.loopForever();

  grpc_thread.join();
  return 0;
}
