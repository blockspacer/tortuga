use std::collections::HashMap;

use ::google::empty::Empty;
use ::tortuga::module::TortugaModule;

#[derive(Clone)]
pub struct TortugaService {
    modules: HashMap<String, TortugaModule>,
}

impl crate::proto::tortuga_grpc::Tortuga for TortugaService {
fn create_task(&mut self, ctx: ::grpcio::RpcContext, req: crate::proto::tortuga::CreateReq, sink: ::grpcio::UnarySink<crate::proto::tortuga::CreateResp>) {}
     fn request_task(&mut self, ctx: ::grpcio::RpcContext, req: crate::proto::tortuga::TaskReq, sink: ::grpcio::UnarySink<crate::proto::tortuga::TaskResp>){}                   
     fn heartbeat(&mut self, ctx: ::grpcio::RpcContext, req: crate::proto::tortuga::HeartbeatReq, sink: ::grpcio::UnarySink<Empty>){}                     
     fn complete_task(&mut self, ctx: ::grpcio::RpcContext, req: crate::proto::tortuga::CompleteTaskReq, sink: ::grpcio::UnarySink<Empty>){}               
     fn update_progress(&mut self, ctx: ::grpcio::RpcContext, req: crate::proto::tortuga::UpdateProgressReq, sink: ::grpcio::UnarySink<Empty>){}           
     fn find_task(&mut self, ctx: ::grpcio::RpcContext, req: crate::proto::tortuga::TaskIdentifier, sink: ::grpcio::UnarySink<crate::proto::tortuga::TaskProgress>){}          
     fn find_task_by_handle(&mut self, ctx: ::grpcio::RpcContext, req: crate::proto::tortuga::FindTaskReq, sink: ::grpcio::UnarySink<crate::proto::tortuga::TaskProgress>){}    
     fn ping(&mut self, ctx: ::grpcio::RpcContext, req: Empty, sink: ::grpcio::UnarySink<Empty>){}                                   
     fn quit_quit_quit(&mut self, ctx: ::grpcio::RpcContext, req: Empty, sink: ::grpcio::UnarySink<Empty>){}                          
}
