// The utility of a baton handler is to be the "tag" in the GRPC async service,
// which waits on a fiber, thus effectively integrating GRPC async and folly fibers.
#pragma once

#include "boost/utility.hpp"
#include "folly/fibers/Baton.h"

namespace tortuga {
using folly::fibers::Baton;

class BatonHandler : boost::noncopyable {
 public:
  BatonHandler();
  ~BatonHandler();

  void Handle(bool ok);

  bool Wait();
  void Reset();

 private:
  Baton bat_;
  bool ok_{ false };
};
}  // namespace tortuga
