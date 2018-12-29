#pragma once

#include <stdint.h>

#include "google/protobuf/timestamp.pb.h"
#include "grpc++/grpc++.h"

namespace tortuga {
// Current millis since epoch.
int64_t CurrentTimeMillis();
int64_t CurrentTimeMicros();

google::protobuf::Timestamp FromEpochMillis(int64_t millis);

void SetDeadlineMillis(int millis, grpc::ClientContext* ctx);
}  // namespace tortuga
