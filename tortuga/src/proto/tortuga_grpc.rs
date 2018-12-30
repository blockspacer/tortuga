// This file is generated. Do not edit
// @generated

// https://github.com/Manishearth/rust-clippy/issues/702
#![allow(unknown_lints)]
#![allow(clippy)]

#![cfg_attr(rustfmt, rustfmt_skip)]

#![allow(box_pointers)]
#![allow(dead_code)]
#![allow(missing_docs)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(non_upper_case_globals)]
#![allow(trivial_casts)]
#![allow(unsafe_code)]
#![allow(unused_imports)]
#![allow(unused_results)]

const METHOD_TORTUGA_CREATE_TASK: ::grpcio::Method<super::tortuga::CreateReq, super::tortuga::CreateResp> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/tortuga.Tortuga/CreateTask",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_TORTUGA_REQUEST_TASK: ::grpcio::Method<super::tortuga::TaskReq, super::tortuga::TaskResp> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/tortuga.Tortuga/RequestTask",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_TORTUGA_HEARTBEAT: ::grpcio::Method<super::tortuga::HeartbeatReq, super::empty::Empty> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/tortuga.Tortuga/Heartbeat",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_TORTUGA_COMPLETE_TASK: ::grpcio::Method<super::tortuga::CompleteTaskReq, super::empty::Empty> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/tortuga.Tortuga/CompleteTask",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_TORTUGA_UPDATE_PROGRESS: ::grpcio::Method<super::tortuga::UpdateProgressReq, super::empty::Empty> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/tortuga.Tortuga/UpdateProgress",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_TORTUGA_FIND_TASK: ::grpcio::Method<super::tortuga::TaskIdentifier, super::tortuga::TaskProgress> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/tortuga.Tortuga/FindTask",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_TORTUGA_FIND_TASK_BY_HANDLE: ::grpcio::Method<super::tortuga::FindTaskReq, super::tortuga::TaskProgress> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/tortuga.Tortuga/FindTaskByHandle",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_TORTUGA_PING: ::grpcio::Method<super::empty::Empty, super::empty::Empty> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/tortuga.Tortuga/Ping",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_TORTUGA_QUIT_QUIT_QUIT: ::grpcio::Method<super::empty::Empty, super::empty::Empty> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/tortuga.Tortuga/QuitQuitQuit",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

pub struct TortugaClient {
    client: ::grpcio::Client,
}

impl TortugaClient {
    pub fn new(channel: ::grpcio::Channel) -> Self {
        TortugaClient {
            client: ::grpcio::Client::new(channel),
        }
    }

    pub fn create_task_opt(&self, req: &super::tortuga::CreateReq, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::tortuga::CreateResp> {
        self.client.unary_call(&METHOD_TORTUGA_CREATE_TASK, req, opt)
    }

    pub fn create_task(&self, req: &super::tortuga::CreateReq) -> ::grpcio::Result<super::tortuga::CreateResp> {
        self.create_task_opt(req, ::grpcio::CallOption::default())
    }

    pub fn create_task_async_opt(&self, req: &super::tortuga::CreateReq, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::tortuga::CreateResp>> {
        self.client.unary_call_async(&METHOD_TORTUGA_CREATE_TASK, req, opt)
    }

    pub fn create_task_async(&self, req: &super::tortuga::CreateReq) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::tortuga::CreateResp>> {
        self.create_task_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn request_task_opt(&self, req: &super::tortuga::TaskReq, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::tortuga::TaskResp> {
        self.client.unary_call(&METHOD_TORTUGA_REQUEST_TASK, req, opt)
    }

    pub fn request_task(&self, req: &super::tortuga::TaskReq) -> ::grpcio::Result<super::tortuga::TaskResp> {
        self.request_task_opt(req, ::grpcio::CallOption::default())
    }

    pub fn request_task_async_opt(&self, req: &super::tortuga::TaskReq, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::tortuga::TaskResp>> {
        self.client.unary_call_async(&METHOD_TORTUGA_REQUEST_TASK, req, opt)
    }

    pub fn request_task_async(&self, req: &super::tortuga::TaskReq) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::tortuga::TaskResp>> {
        self.request_task_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn heartbeat_opt(&self, req: &super::tortuga::HeartbeatReq, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::empty::Empty> {
        self.client.unary_call(&METHOD_TORTUGA_HEARTBEAT, req, opt)
    }

    pub fn heartbeat(&self, req: &super::tortuga::HeartbeatReq) -> ::grpcio::Result<super::empty::Empty> {
        self.heartbeat_opt(req, ::grpcio::CallOption::default())
    }

    pub fn heartbeat_async_opt(&self, req: &super::tortuga::HeartbeatReq, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.client.unary_call_async(&METHOD_TORTUGA_HEARTBEAT, req, opt)
    }

    pub fn heartbeat_async(&self, req: &super::tortuga::HeartbeatReq) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.heartbeat_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn complete_task_opt(&self, req: &super::tortuga::CompleteTaskReq, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::empty::Empty> {
        self.client.unary_call(&METHOD_TORTUGA_COMPLETE_TASK, req, opt)
    }

    pub fn complete_task(&self, req: &super::tortuga::CompleteTaskReq) -> ::grpcio::Result<super::empty::Empty> {
        self.complete_task_opt(req, ::grpcio::CallOption::default())
    }

    pub fn complete_task_async_opt(&self, req: &super::tortuga::CompleteTaskReq, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.client.unary_call_async(&METHOD_TORTUGA_COMPLETE_TASK, req, opt)
    }

    pub fn complete_task_async(&self, req: &super::tortuga::CompleteTaskReq) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.complete_task_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn update_progress_opt(&self, req: &super::tortuga::UpdateProgressReq, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::empty::Empty> {
        self.client.unary_call(&METHOD_TORTUGA_UPDATE_PROGRESS, req, opt)
    }

    pub fn update_progress(&self, req: &super::tortuga::UpdateProgressReq) -> ::grpcio::Result<super::empty::Empty> {
        self.update_progress_opt(req, ::grpcio::CallOption::default())
    }

    pub fn update_progress_async_opt(&self, req: &super::tortuga::UpdateProgressReq, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.client.unary_call_async(&METHOD_TORTUGA_UPDATE_PROGRESS, req, opt)
    }

    pub fn update_progress_async(&self, req: &super::tortuga::UpdateProgressReq) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.update_progress_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn find_task_opt(&self, req: &super::tortuga::TaskIdentifier, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::tortuga::TaskProgress> {
        self.client.unary_call(&METHOD_TORTUGA_FIND_TASK, req, opt)
    }

    pub fn find_task(&self, req: &super::tortuga::TaskIdentifier) -> ::grpcio::Result<super::tortuga::TaskProgress> {
        self.find_task_opt(req, ::grpcio::CallOption::default())
    }

    pub fn find_task_async_opt(&self, req: &super::tortuga::TaskIdentifier, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::tortuga::TaskProgress>> {
        self.client.unary_call_async(&METHOD_TORTUGA_FIND_TASK, req, opt)
    }

    pub fn find_task_async(&self, req: &super::tortuga::TaskIdentifier) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::tortuga::TaskProgress>> {
        self.find_task_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn find_task_by_handle_opt(&self, req: &super::tortuga::FindTaskReq, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::tortuga::TaskProgress> {
        self.client.unary_call(&METHOD_TORTUGA_FIND_TASK_BY_HANDLE, req, opt)
    }

    pub fn find_task_by_handle(&self, req: &super::tortuga::FindTaskReq) -> ::grpcio::Result<super::tortuga::TaskProgress> {
        self.find_task_by_handle_opt(req, ::grpcio::CallOption::default())
    }

    pub fn find_task_by_handle_async_opt(&self, req: &super::tortuga::FindTaskReq, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::tortuga::TaskProgress>> {
        self.client.unary_call_async(&METHOD_TORTUGA_FIND_TASK_BY_HANDLE, req, opt)
    }

    pub fn find_task_by_handle_async(&self, req: &super::tortuga::FindTaskReq) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::tortuga::TaskProgress>> {
        self.find_task_by_handle_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn ping_opt(&self, req: &super::empty::Empty, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::empty::Empty> {
        self.client.unary_call(&METHOD_TORTUGA_PING, req, opt)
    }

    pub fn ping(&self, req: &super::empty::Empty) -> ::grpcio::Result<super::empty::Empty> {
        self.ping_opt(req, ::grpcio::CallOption::default())
    }

    pub fn ping_async_opt(&self, req: &super::empty::Empty, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.client.unary_call_async(&METHOD_TORTUGA_PING, req, opt)
    }

    pub fn ping_async(&self, req: &super::empty::Empty) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.ping_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn quit_quit_quit_opt(&self, req: &super::empty::Empty, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::empty::Empty> {
        self.client.unary_call(&METHOD_TORTUGA_QUIT_QUIT_QUIT, req, opt)
    }

    pub fn quit_quit_quit(&self, req: &super::empty::Empty) -> ::grpcio::Result<super::empty::Empty> {
        self.quit_quit_quit_opt(req, ::grpcio::CallOption::default())
    }

    pub fn quit_quit_quit_async_opt(&self, req: &super::empty::Empty, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.client.unary_call_async(&METHOD_TORTUGA_QUIT_QUIT_QUIT, req, opt)
    }

    pub fn quit_quit_quit_async(&self, req: &super::empty::Empty) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.quit_quit_quit_async_opt(req, ::grpcio::CallOption::default())
    }
    pub fn spawn<F>(&self, f: F) where F: ::futures::Future<Item = (), Error = ()> + Send + 'static {
        self.client.spawn(f)
    }
}

pub trait Tortuga {
    fn create_task(&mut self, ctx: ::grpcio::RpcContext, req: super::tortuga::CreateReq, sink: ::grpcio::UnarySink<super::tortuga::CreateResp>);
    fn request_task(&mut self, ctx: ::grpcio::RpcContext, req: super::tortuga::TaskReq, sink: ::grpcio::UnarySink<super::tortuga::TaskResp>);
    fn heartbeat(&mut self, ctx: ::grpcio::RpcContext, req: super::tortuga::HeartbeatReq, sink: ::grpcio::UnarySink<super::empty::Empty>);
    fn complete_task(&mut self, ctx: ::grpcio::RpcContext, req: super::tortuga::CompleteTaskReq, sink: ::grpcio::UnarySink<super::empty::Empty>);
    fn update_progress(&mut self, ctx: ::grpcio::RpcContext, req: super::tortuga::UpdateProgressReq, sink: ::grpcio::UnarySink<super::empty::Empty>);
    fn find_task(&mut self, ctx: ::grpcio::RpcContext, req: super::tortuga::TaskIdentifier, sink: ::grpcio::UnarySink<super::tortuga::TaskProgress>);
    fn find_task_by_handle(&mut self, ctx: ::grpcio::RpcContext, req: super::tortuga::FindTaskReq, sink: ::grpcio::UnarySink<super::tortuga::TaskProgress>);
    fn ping(&mut self, ctx: ::grpcio::RpcContext, req: super::empty::Empty, sink: ::grpcio::UnarySink<super::empty::Empty>);
    fn quit_quit_quit(&mut self, ctx: ::grpcio::RpcContext, req: super::empty::Empty, sink: ::grpcio::UnarySink<super::empty::Empty>);
}

pub fn create_tortuga<S: Tortuga + Send + Clone + 'static>(s: S) -> ::grpcio::Service {
    let mut builder = ::grpcio::ServiceBuilder::new();
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_TORTUGA_CREATE_TASK, move |ctx, req, resp| {
        instance.create_task(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_TORTUGA_REQUEST_TASK, move |ctx, req, resp| {
        instance.request_task(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_TORTUGA_HEARTBEAT, move |ctx, req, resp| {
        instance.heartbeat(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_TORTUGA_COMPLETE_TASK, move |ctx, req, resp| {
        instance.complete_task(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_TORTUGA_UPDATE_PROGRESS, move |ctx, req, resp| {
        instance.update_progress(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_TORTUGA_FIND_TASK, move |ctx, req, resp| {
        instance.find_task(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_TORTUGA_FIND_TASK_BY_HANDLE, move |ctx, req, resp| {
        instance.find_task_by_handle(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_TORTUGA_PING, move |ctx, req, resp| {
        instance.ping(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_TORTUGA_QUIT_QUIT_QUIT, move |ctx, req, resp| {
        instance.quit_quit_quit(ctx, req, resp)
    });
    builder.build()
}
