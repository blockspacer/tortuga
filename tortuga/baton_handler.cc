#include "tortuga/baton_handler.h"

namespace tortuga {
BatonHandler::BatonHandler() {
}

BatonHandler::~BatonHandler() {
}

void BatonHandler::Handle(bool ok) {
  ok_ = ok;
  bat_.post();
}

bool BatonHandler::Wait() {
  bat_.wait();
  return ok_;
}

void BatonHandler::Reset() {
  bat_.reset();
  ok_ = false;
}
}  // tortuga
