#include "tortuga/workers_manager.h"

#include "folly/Conv.h"
#include "folly/MapUtil.h"
#include "folly/String.h"
#include "folly/fibers/Promise.h"
#include "glog/logging.h"

#include "tortuga/fields.h"
#include "tortuga/sqlite_statement.h"
#include "tortuga/time_logger.h"
#include "tortuga/time_utils.h"

namespace tortuga {
namespace {
static const char* const kSelectTasksWorkedOnStmt = R"(
  select rowid, * from tasks where worked_on = 1 and done = 0;
)";

static const char* const kSelectWorkerByUuidStmt = R"(
  select * from workers where uuid = ?;
)";

static const char* const kSelectWorkerUuidStmt = "select uuid from workers where worker_id = ? LIMIT 1";

static const char* const kUpdateWorkerBeatStmt = R"(
  update workers set last_beat=? where worker_id=? ;
)";

static const char* const kUpdateWorkerStmt = R"(
  update workers set uuid=?, capabilities=?, last_beat=? where worker_id=? ;
)";

static const char* const kInsertWorkerStmt = R"(
  insert into workers (uuid, worker_id, capabilities, last_beat) values (?, ?, ?, ?);
)";

static const char* const kInsertHistoricWorkerStmt = R"(
    insert into historic_workers(uuid, worker_id, created) values (?, ?, ?);
)";

static const char* const kUnassignTasksStmt = R"(
    update tasks set
    worked_on=0,
    worker_uuid=NULL
    where worker_uuid=? and done=0;
)";

static const char* const kUnassignSingleTaskByRowidStmt = R"(
    update tasks set
    worked_on=0,
    worker_uuid=NULL
    where rowid = ?;
)";

static const char* const kUpdateWorkerInvalidatedUuidStmt = R"(
    update workers set last_invalidated_uuid=? where uuid=?;
)";

static const char* const kSelectExpiredWorkersStmt = R"(
    select uuid, last_invalidated_uuid from workers where last_beat < ?;
)";

std::string JoinCapabilities(const Worker& worker) {
  return folly::join(" ", worker.capabilities());
}
}  // anonymous namespace

WorkersManager::WorkersManager(sqlite3* db,
                               folly::CPUThreadPoolExecutor* exec, 
                               OnWorkerDeath on_worker_death)
    : db_(db),
      exec_(exec),
      select_worker_uuid_stmt_(db, kSelectWorkerUuidStmt),
      update_worker_beat_stmt_(db, kUpdateWorkerBeatStmt),
      update_worker_stmt_(db, kUpdateWorkerStmt),
      insert_worker_stmt_(db, kInsertWorkerStmt),
      insert_historic_worker_stmt_(db, kInsertHistoricWorkerStmt),
      unassign_tasks_stmt_(db, kUnassignTasksStmt),
      update_worker_invalidated_uuid_stmt_(db, kUpdateWorkerInvalidatedUuidStmt),
      select_expired_workers_stmt_(db, kSelectExpiredWorkersStmt),
      on_worker_death_(std::move(on_worker_death)) {
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
    CHECK(stepped == SQLITE_ROW || stepped == SQLITE_DONE) << "sql err: " << stepped
        << " err msg: " << sqlite3_errmsg(db_);
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
      if (worker_info == nullptr) {
        std::string id = select_task_worker.ColumnText(WorkersFields::kWorkerId);
        WorkerInfo& w = workers_[id];
        w.uuid = uuid;
        w.id = id;
        w.last_beat_millis = CurrentTimeMillis();
      }
    }
  }

  LOG(INFO) << "successfully loaded: " << workers_.size() << " workers";
  SqliteStatement unassign(db_, kUnassignSingleTaskByRowidStmt);
  for (int64_t row_id : to_unassign) {
    SqliteReset x(&unassign);
    unassign.BindLong(1, row_id);
    unassign.ExecuteOrDie();
    LOG(INFO) << "unassigned orphan: " << row_id;
  }
}

void WorkersManager::InsertHistoricWorkerInExec(const std::string& uuid,
                                                const std::string& worker_id) {
  SqliteReset x(&insert_historic_worker_stmt_);
  insert_historic_worker_stmt_.BindText(1, uuid);
  insert_historic_worker_stmt_.BindText(2, worker_id);
  insert_historic_worker_stmt_.BindLong(3, CurrentTimeMillis());

  insert_historic_worker_stmt_.ExecuteOrDie();
}

void WorkersManager::UnassignTaskInExec(const std::string& uuid) {
  VLOG(2) << "unassigning tasks of expired worker: " << uuid;

  SqliteTx tx(db_);
  SqliteReset x1(&unassign_tasks_stmt_);
  unassign_tasks_stmt_.BindText(1, uuid);
  unassign_tasks_stmt_.ExecuteOrDie();

  SqliteReset x2(&update_worker_invalidated_uuid_stmt_);
  update_worker_invalidated_uuid_stmt_.BindText(1, uuid);
  update_worker_invalidated_uuid_stmt_.BindText(2, uuid);
  update_worker_invalidated_uuid_stmt_.ExecuteOrDie();

  on_worker_death_(uuid);
}

void WorkersManager::CheckHeartbeats() {
  VLOG(3) << "checking heartbeats";
  int64_t expired_millis = CurrentTimeMillis() - 30000L;
  // uuids of expired workers.
  std::vector<std::string> uuids;

  auto it = workers_.begin();
  while (it != workers_.end()) {
    WorkerInfo& info = it->second;
    if (info.last_beat_millis < expired_millis) {
      uuids.push_back(info.uuid);
      auto toErase = it;
      ++it;
      workers_.erase(toErase);
    } else {
       ++it;
    }
  }

  folly::fibers::await([&](folly::fibers::Promise<folly::Unit> p) {
    exec_->add([this, promise = std::move(p), exp_uuids = std::move(uuids)]() mutable {
      UnassignTasksInExec(exp_uuids);
      promise.setValue(folly::Unit());
    });
  });

  VLOG(3) << "done checking heartbeats...";
}

void WorkersManager::UnassignTasksInExec(const std::vector<std::string>& uuids) {
  for (const auto& uuid : uuids) {
    UnassignTaskInExec(uuid);
  }
}

void WorkersManager::Beat(const Worker& worker) {
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

void WorkersManager::RegularBeat(const Worker& worker, WorkerInfo* worker_info) {
  VLOG(3) << "beating worker " << worker.uuid() << " is known and uptodate";
  worker_info->last_beat_millis = CurrentTimeMillis();
}

void WorkersManager::WorkerChangeBeat(const Worker& worker,  WorkerInfo* worker_info) {
  LOG(INFO) << "beating worker " << worker.uuid() << " has changed uuid.";

  folly::fibers::await([&](folly::fibers::Promise<folly::Unit> p) {
    exec_->add([this, promise = std::move(p), old_uuid = worker_info->uuid, &worker]() mutable {
      // invalidate all the tasks assigned to this whole worker.
      UnassignTaskInExec(old_uuid);
      // insert the new one
      SqliteReset x2(&update_worker_stmt_);
      update_worker_stmt_.BindText(1, worker.uuid());
      update_worker_stmt_.BindText(2, JoinCapabilities(worker));
      update_worker_stmt_.BindLong(3, CurrentTimeMillis());
      update_worker_stmt_.BindText(4, worker.worker_id());

      update_worker_stmt_.ExecuteOrDie();
      
      InsertHistoricWorkerInExec(worker.uuid(), worker.worker_id());
      promise.setValue(folly::Unit());
    });
  });

  worker_info->uuid = worker.uuid();
  worker_info->last_beat_millis = CurrentTimeMillis();
}

void WorkersManager::NewWorkerBeat(const Worker& worker) {
  LOG(INFO) << "we are welcoming a new worker to the cluster! " << worker.ShortDebugString();

  folly::fibers::await([&](folly::fibers::Promise<folly::Unit> p) {
    exec_->add([this, promise = std::move(p), &worker]() mutable {
      SqliteReset x2(&insert_worker_stmt_);
      insert_worker_stmt_.BindText(1, worker.uuid());
      insert_worker_stmt_.BindText(2, worker.worker_id());
      insert_worker_stmt_.BindText(3, JoinCapabilities(worker));
      insert_worker_stmt_.BindLong(4, CurrentTimeMillis());
      insert_worker_stmt_.ExecuteOrDie();

      InsertHistoricWorkerInExec(worker.uuid(), worker.worker_id());
      promise.setValue(folly::Unit());
    });
  });

  WorkerInfo& new_info = workers_[worker.worker_id()];
  new_info.id = worker.worker_id();
  new_info.uuid = worker.uuid();
  new_info.last_beat_millis = CurrentTimeMillis();
}

bool WorkersManager::IsKnownWorker(const Worker& worker) {
  WorkerInfo* worker_info = folly::get_ptr(workers_, worker.worker_id());
  if (worker_info == nullptr) {
    return false;
  }

  return worker_info->uuid == worker.uuid();
}
}  // namespace tortuga
