#include "tortuga/workers_cache.h"

namespace tortuga {
WorkersCache::WorkersCache(sqlite3* db, folly::CPUThreadPoolExecutor* exec)
    : db_(db),
      exec_(exec) {
}

WorkersCache::~WorkersCache() {

}

void WorkersCache::Beat(const Worker& worker) {

}
}  // namespace tortuga
