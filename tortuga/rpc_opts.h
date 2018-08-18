#pragma once

#include "folly/fibers/FiberManager.h"
#include "grpc++/grpc++.h"

#include "tortuga/tortuga.grpc.pb.h"
#include "tortuga/tortuga.pb.h"

namespace tortuga {
struct RpcOpts {
  Tortuga::AsyncService* tortuga_grpc{ nullptr };
  grpc::ServerCompletionQueue* cq{ nullptr };
  folly::fibers::FiberManager* fibers{ nullptr };
};
}  // namespace tortuga
