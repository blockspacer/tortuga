extern crate getopts;
#[macro_use] extern crate log;
extern crate simplelog;
extern crate rusqlite;

use getopts::Options;
use rusqlite::*;

pub fn main() {
  let stdout_logger = simplelog::TermLogger::new(simplelog::LevelFilter::Info, simplelog::Config::default()).unwrap();
  simplelog::CombinedLogger::init(vec![stdout_logger]).unwrap();

  let args: Vec<String> = std::env::args().collect();

  let mut opts = Options::new();
  opts.optopt("d", "db", "Tortuga DB file", "tortuga.db");

  let matches = match opts.parse(&args[1..]) {
    Ok(m) => { m }
    Err(f) => { panic!(f.to_string()) }
  };

  let db = matches.opt_str("d").unwrap_or("tortuga.db".to_string());
  info!("using sqlite3 tortuga database: {}", db);

  let conn = Connection::open(db).unwrap();
  
  info!("creating SQLITE tables.");
  let create_tables = r#"
CREATE TABLE IF NOT EXISTS tasks(
    id TEXT NOT NULL,
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

CREATE INDEX IF NOT EXISTS tasks_id_idx ON tasks (id);

CREATE TABLE IF NOT EXISTS workers(
  worker_id TEXT NOT NULL,
  uuid TEXT NOT NULL,
  capabilities TEXT NOT NULL DEFAULT '',
  last_beat INTEGER NOT NULL,
  last_invalidated_uuid TEXT NULL DEFAULT NULL
);

CREATE INDEX IF NOT EXISTS workers_worker_id_idx ON workers (worker_id);

CREATE TABLE IF NOT EXISTS historic_workers(
  uuid TEXT NOT NULL,
  worker_id TEXT NOT NULL,
  created INTEGER NOT NULL
);

CREATE INDEX IF NOT EXISTS historic_workers_uuid_idx ON historic_workers (uuid);
"#;

  conn.execute(create_tables, &[]).unwrap();
  // This makes sqlite inserts much faster.
  conn.execute_batch("PRAGMA journal_mode = WAL").unwrap();
  conn.execute_batch("PRAGMA synchronous = NORMAL").unwrap();
}
