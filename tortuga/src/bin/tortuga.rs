use std::sync::{Arc, Barrier};

use diesel::prelude::*;
use grpcio::{Environment, ServerBuilder};
use log::info;

pub fn main() {
    ::simplelog::CombinedLogger::init(vec![
        ::simplelog::TermLogger::new(
            ::simplelog::LevelFilter::Info,
            ::simplelog::Config::default(),
        )
        .unwrap(),
    ])
    .unwrap();
    
    let database_url = "tortuga/data/tortuga.db";
    let conn = SqliteConnection::establish(&database_url)
        .unwrap();

 {
     // TODO(christian) cleanup this test
    let new_task = tortuga::models::NewHistoricWorker {
       uuid: "id_1",
       worker_id: "TypeOfTask",
       created: 12345_i32,
    };

    diesel::insert_into(tortuga::schema::historic_workers::table)
        .values(&new_task)
        .execute(&conn)
        .expect("Error saving new task");
 }

    let env = Arc::new(Environment::new(2));
    let instance = tortuga::tortuga_grpc_impl::TortugaService {
    };

    let service = tortuga::proto::tortuga_grpc::create_tortuga(instance);
    let mut server = ServerBuilder::new(env)
        .register_service(service)
        .bind("127.0.0.1", 50_650)
        .build()
        .unwrap();

    server.start();
    info!("Tortuga took the stage on port: 50640, powered by rust!");

    // run forever :)
    let barrier = Arc::new(Barrier::new(2));
    barrier.wait();

    ()
}
