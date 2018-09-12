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
#include "sqlite3.h"

#include "tortuga/baton_handler.h"
#include "tortuga/module.h"
#include "tortuga/rpc_opts.h"
#include "tortuga/tortuga.h"
#include "tortuga/tortuga.grpc.pb.h"
#include "tortuga/tortuga.pb.h"
#include "tortuga/modules/firestore.h"

DEFINE_string(db_file, "tortuga.db", "path to db file.");
DEFINE_string(addr, "127.0.0.1", "address to listen on.");
DEFINE_int32(port, 4000, "port to listen on");

// all our times are in millis since epoch.
const char* const kCreateTortuga = R"(
  CREATE TABLE IF NOT EXISTS tasks(
    id TEXT NOT NULL,
    task_type TEXT NOT NULL,
    data BLOB NOT NULL,
    created INTEGER NOT NULL,
    max_retries INTEGER NOT NULL,
    retries INTEGER NOT NULL DEFAULT 0,
    priority INTEGER NOT NULL,
    delayed_time INTEGER NULL DEFAULT NULL,
    modules TEXT NULL DEFAULT NULL,
    worked_on BOOLEAN NOT NULL DEFAULT false,
    worker_uuid TEXT NULL DEFAULT NULL,
    progress FLOAT NOT NULL DEFAULT 0.0,
    progress_message TEXT NULL,
    status_code INTEGER NULL DEFAULT NULL,
    status_message TEXT NULL DEFAULT NULL,
    done BOOLEAN NOT NULL DEFAULT false,
    started_time INTEGER NULL DEFAULT NULL,
    done_time INTEGER NULL DEFAULT NULL,
    logs TEXT NULL
  );

  CREATE INDEX IF NOT EXISTS tasks_id_idx ON tasks (id);

  CREATE TABLE IF NOT EXISTS workers(
    worker_id TEXT NOT NULL,
    uuid TEXT NOT NULL,
    capabilities TEXT NOT NULL DEFAULT '',
    last_beat INTEGER NOT NULL,
    last_invalidated_uuid TEXT NULL DEFAULT NULL
  );

  CREATE INDEX IF NOT EXISTS workers_worker_id_idx ON workers (worker_id);

  CREATE TABLE IF NOT EXISTS historic_workers(
    uuid TEXT NOT NULL,
    worker_id TEXT NOT NULL,
    created INTEGER NOT NULL
  );

  CREATE INDEX IF NOT EXISTS historic_workers_uuid_idx ON historic_workers (uuid);
)";

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

  sqlite3* db = nullptr;
  char* err_msg = nullptr;
  int rc = sqlite3_open(FLAGS_db_file.c_str(), &db);
  CHECK_EQ(SQLITE_OK, rc) << "cound't open database "
                          << FLAGS_db_file << ": " << sqlite3_errmsg(db);

  rc = sqlite3_exec(db, kCreateTortuga, nullptr, nullptr, &err_msg);
  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    LOG(FATAL) << "couldn't create tortuga database: " << err_msg;
  }

  // This makes sqlite inserts much faster.
  CHECK_EQ(SQLITE_OK, sqlite3_exec(db, "PRAGMA journal_mode = WAL", nullptr, nullptr, &err_msg)) << err_msg;
  CHECK_EQ(SQLITE_OK, sqlite3_exec(db, "PRAGMA synchronous = NORMAL", nullptr, nullptr, &err_msg)) << err_msg;

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
  modules["firestore"] = std::make_unique<tortuga::FirestoreModule>();

  tortuga::TortugaHandler tortuga(db, rpc_opts, std::move(modules));

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
  sqlite3_close(db);
  return 0;
}
