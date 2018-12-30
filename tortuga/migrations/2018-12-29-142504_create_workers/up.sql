CREATE TABLE  workers(
  id INTEGER PRIMARY KEY,
  worker_id TEXT NOT NULL,
  uuid TEXT NOT NULL,
  capabilities TEXT NOT NULL DEFAULT '',
  last_beat INTEGER NOT NULL,
  last_invalidated_uuid TEXT NULL DEFAULT NULL
);

CREATE INDEX IF NOT EXISTS workers_worker_id_idx ON workers (worker_id);

CREATE TABLE historic_workers(
  id INTEGER PRIMARY KEY,
  uuid TEXT NOT NULL,
  worker_id TEXT NOT NULL,
  created INTEGER NOT NULL
);

CREATE INDEX IF NOT EXISTS historic_workers_uuid_idx ON historic_workers (uuid);
