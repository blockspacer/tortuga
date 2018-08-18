#include "tortuga/time_utils.h"

#include <chrono>

namespace tortuga {
int64_t CurrentTimeMillis() {
  auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
  return now_ms.count();
}

google::protobuf::Timestamp FromEpochMillis(int64_t millis) {
  google::protobuf::Timestamp t;
  t.set_seconds(millis / 1000L);
  t.set_nanos(static_cast<int32_t>((millis % 1000L) * 1000000L));

  return t;
}
}  // namespace tortuga
