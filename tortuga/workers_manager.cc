#include "tortuga/workers_manager.h"

#include "folly/Conv.h"
#include "folly/MapUtil.h"
#include "glog/logging.h"

#include "tortuga/fields.h"
#include "tortuga/sqlite_statement.h"
#include "tortuga/time_utils.h"

namespace tortuga {
namespace {
static const char* const kSelectTasksWorkedOnStmt = R"(
  select rowid, * from tasks where worked_on = 1 and done = 0;
)";

static const char* const kSelectWorkerByUuidStmt = R"(
  select * from workers where uuid = ?;
)";
}  // anonymous namespace

WorkersManager::WorkersManager(sqlite3* db, folly::CPUThreadPoolExecutor* exec)
    : db_(db),
      exec_(exec) {
}

WorkersManager::~WorkersManager() {
  db_ = nullptr;
  exec_ = nullptr;
}

void WorkersManager::LoadWorkers() {
  LOG(INFO) << "rebuilding workers cache from base.";
  std::vector<int64_t> to_unassign;
  SqliteStatement tasks(db_, kSelectTasksWorkedOnStmt);
  
  for (;;) {
    int stepped = tasks.Step();
    CHECK(stepped == SQLITE_ROW || stepped == SQLITE_DONE);
    if (stepped == SQLITE_DONE) {
      break;
    }

    const std::string uuid = tasks.ColumnTextOrEmpty(TasksFields::kWorkerUuid + 1);  //  +1 cause we selected rowid
    const int64_t row_id = tasks.ColumnLong(0);

    SqliteStatement select_task_worker(db_, kSelectWorkerByUuidStmt);
    select_task_worker.BindText(1, uuid);
    int found_worker = select_task_worker.Step();
    CHECK(found_worker == SQLITE_ROW || found_worker == SQLITE_DONE);
    if (found_worker == SQLITE_DONE) {
      LOG(WARNING) << "Task: " << row_id << " belonging to unknown worker: " << uuid << ", we will unassign";
      to_unassign.push_back(row_id);
    } else {
      WorkerInfo* worker_info = folly::get_ptr(workers_, uuid);
      if (worker_info != nullptr) {
      } else {
        std::string id = select_task_worker.ColumnText(WorkersFields::kWorkerId);
        WorkerInfo& w = workers_[id];
        w.uuid = uuid;
        w.id = id;
        w.last_beat_millis = CurrentTimeMillis();
      }
    }
  }

  LOG(INFO) << "successfully loaded: " << workers_.size() << " workers";
}

/*
void WorkersCache::Beat(const Worker& worker) {
  WorkerInfo* worker_info = folly::get_ptr(workers_, worker.worker_id());
  if (worker_info != nullptr) {
    if (worker_info->uuid == worker.uuid()) {
      RegularBeat(worker, worker_info);
    } else {
      WorkerChangeBeat(worker, worker_info);
    }
  } else {
    NewWorkerBeat(worker);
  }
}

void WorkersCache::RegularBeat(const Worker& worker, WorkerInfo* worker_info) {
  VLOG(3) << "beating worker " << worker.uuid() << " is known and uptodate";
  worker_info->last_beat_millis = CurrentTimeMillis();
}

void WorkersCache::WorkerChangeBeat(const Worker& worker,  WorkerInfo* worker_info) {

}

void WorkersCache::NewWorkerBeat(const Worker& worker) {

}*/


}  // namespace tortuga
