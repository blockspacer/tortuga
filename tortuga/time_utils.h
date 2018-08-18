#pragma once

#include <stdint.h>

#include "google/protobuf/timestamp.pb.h"

namespace tortuga {
// Current millis since epoch.
int64_t CurrentTimeMillis();

google::protobuf::Timestamp FromEpochMillis(int64_t millis);
}  // namespace tortuga
