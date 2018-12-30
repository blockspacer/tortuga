CREATE TABLE IF NOT EXISTS tasks(
    id INTEGER PRIMARY KEY,
    handle_id TEXT NOT NULL,
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
    progress_metadata TEXT NULL DEFAULT NULL,
    status_code INTEGER NULL DEFAULT NULL,
    status_message TEXT NULL DEFAULT NULL,
    done BOOLEAN NOT NULL DEFAULT false,
    started_time INTEGER NULL DEFAULT NULL,
    done_time INTEGER NULL DEFAULT NULL,
    logs TEXT NULL,
    output TEXT NULL DEFAULT NULL
  );

CREATE INDEX IF NOT EXISTS tasks_handle_id_idx ON tasks (handle_id);
