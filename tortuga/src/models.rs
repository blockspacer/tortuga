use super::schema::historic_workers;
use super::schema::tasks;

#[derive(Queryable)]
pub struct Task {
    pub id: i64,
    pub handle_id: String,
    pub task_type: String,
}

#[derive(Insertable)]
#[table_name="tasks"]
pub struct NewTask<'a> {
    pub handle_id: &'a str,
    pub task_type: &'a str,
}

#[derive(Insertable)]
#[table_name="historic_workers"]
pub struct NewHistoricWorker<'a> {
    pub uuid: &'a str,
    pub worker_id: &'a str,
    pub created: i32,
}
