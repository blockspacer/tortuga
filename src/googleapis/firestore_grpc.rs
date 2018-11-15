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

const METHOD_FIRESTORE_GET_DOCUMENT: ::grpcio::Method<super::firestore::GetDocumentRequest, super::document::Document> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/google.firestore.v1beta1.Firestore/GetDocument",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_FIRESTORE_LIST_DOCUMENTS: ::grpcio::Method<super::firestore::ListDocumentsRequest, super::firestore::ListDocumentsResponse> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/google.firestore.v1beta1.Firestore/ListDocuments",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_FIRESTORE_CREATE_DOCUMENT: ::grpcio::Method<super::firestore::CreateDocumentRequest, super::document::Document> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/google.firestore.v1beta1.Firestore/CreateDocument",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_FIRESTORE_UPDATE_DOCUMENT: ::grpcio::Method<super::firestore::UpdateDocumentRequest, super::document::Document> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/google.firestore.v1beta1.Firestore/UpdateDocument",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_FIRESTORE_DELETE_DOCUMENT: ::grpcio::Method<super::firestore::DeleteDocumentRequest, super::empty::Empty> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/google.firestore.v1beta1.Firestore/DeleteDocument",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_FIRESTORE_BATCH_GET_DOCUMENTS: ::grpcio::Method<super::firestore::BatchGetDocumentsRequest, super::firestore::BatchGetDocumentsResponse> = ::grpcio::Method {
    ty: ::grpcio::MethodType::ServerStreaming,
    name: "/google.firestore.v1beta1.Firestore/BatchGetDocuments",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_FIRESTORE_BEGIN_TRANSACTION: ::grpcio::Method<super::firestore::BeginTransactionRequest, super::firestore::BeginTransactionResponse> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/google.firestore.v1beta1.Firestore/BeginTransaction",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_FIRESTORE_COMMIT: ::grpcio::Method<super::firestore::CommitRequest, super::firestore::CommitResponse> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/google.firestore.v1beta1.Firestore/Commit",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_FIRESTORE_ROLLBACK: ::grpcio::Method<super::firestore::RollbackRequest, super::empty::Empty> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/google.firestore.v1beta1.Firestore/Rollback",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_FIRESTORE_RUN_QUERY: ::grpcio::Method<super::firestore::RunQueryRequest, super::firestore::RunQueryResponse> = ::grpcio::Method {
    ty: ::grpcio::MethodType::ServerStreaming,
    name: "/google.firestore.v1beta1.Firestore/RunQuery",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_FIRESTORE_WRITE: ::grpcio::Method<super::firestore::WriteRequest, super::firestore::WriteResponse> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Duplex,
    name: "/google.firestore.v1beta1.Firestore/Write",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_FIRESTORE_LISTEN: ::grpcio::Method<super::firestore::ListenRequest, super::firestore::ListenResponse> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Duplex,
    name: "/google.firestore.v1beta1.Firestore/Listen",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

const METHOD_FIRESTORE_LIST_COLLECTION_IDS: ::grpcio::Method<super::firestore::ListCollectionIdsRequest, super::firestore::ListCollectionIdsResponse> = ::grpcio::Method {
    ty: ::grpcio::MethodType::Unary,
    name: "/google.firestore.v1beta1.Firestore/ListCollectionIds",
    req_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
    resp_mar: ::grpcio::Marshaller { ser: ::grpcio::pb_ser, de: ::grpcio::pb_de },
};

pub struct FirestoreClient {
    client: ::grpcio::Client,
}

impl FirestoreClient {
    pub fn new(channel: ::grpcio::Channel) -> Self {
        FirestoreClient {
            client: ::grpcio::Client::new(channel),
        }
    }

    pub fn get_document_opt(&self, req: &super::firestore::GetDocumentRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::document::Document> {
        self.client.unary_call(&METHOD_FIRESTORE_GET_DOCUMENT, req, opt)
    }

    pub fn get_document(&self, req: &super::firestore::GetDocumentRequest) -> ::grpcio::Result<super::document::Document> {
        self.get_document_opt(req, ::grpcio::CallOption::default())
    }

    pub fn get_document_async_opt(&self, req: &super::firestore::GetDocumentRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::document::Document>> {
        self.client.unary_call_async(&METHOD_FIRESTORE_GET_DOCUMENT, req, opt)
    }

    pub fn get_document_async(&self, req: &super::firestore::GetDocumentRequest) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::document::Document>> {
        self.get_document_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn list_documents_opt(&self, req: &super::firestore::ListDocumentsRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::firestore::ListDocumentsResponse> {
        self.client.unary_call(&METHOD_FIRESTORE_LIST_DOCUMENTS, req, opt)
    }

    pub fn list_documents(&self, req: &super::firestore::ListDocumentsRequest) -> ::grpcio::Result<super::firestore::ListDocumentsResponse> {
        self.list_documents_opt(req, ::grpcio::CallOption::default())
    }

    pub fn list_documents_async_opt(&self, req: &super::firestore::ListDocumentsRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::firestore::ListDocumentsResponse>> {
        self.client.unary_call_async(&METHOD_FIRESTORE_LIST_DOCUMENTS, req, opt)
    }

    pub fn list_documents_async(&self, req: &super::firestore::ListDocumentsRequest) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::firestore::ListDocumentsResponse>> {
        self.list_documents_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn create_document_opt(&self, req: &super::firestore::CreateDocumentRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::document::Document> {
        self.client.unary_call(&METHOD_FIRESTORE_CREATE_DOCUMENT, req, opt)
    }

    pub fn create_document(&self, req: &super::firestore::CreateDocumentRequest) -> ::grpcio::Result<super::document::Document> {
        self.create_document_opt(req, ::grpcio::CallOption::default())
    }

    pub fn create_document_async_opt(&self, req: &super::firestore::CreateDocumentRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::document::Document>> {
        self.client.unary_call_async(&METHOD_FIRESTORE_CREATE_DOCUMENT, req, opt)
    }

    pub fn create_document_async(&self, req: &super::firestore::CreateDocumentRequest) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::document::Document>> {
        self.create_document_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn update_document_opt(&self, req: &super::firestore::UpdateDocumentRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::document::Document> {
        self.client.unary_call(&METHOD_FIRESTORE_UPDATE_DOCUMENT, req, opt)
    }

    pub fn update_document(&self, req: &super::firestore::UpdateDocumentRequest) -> ::grpcio::Result<super::document::Document> {
        self.update_document_opt(req, ::grpcio::CallOption::default())
    }

    pub fn update_document_async_opt(&self, req: &super::firestore::UpdateDocumentRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::document::Document>> {
        self.client.unary_call_async(&METHOD_FIRESTORE_UPDATE_DOCUMENT, req, opt)
    }

    pub fn update_document_async(&self, req: &super::firestore::UpdateDocumentRequest) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::document::Document>> {
        self.update_document_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn delete_document_opt(&self, req: &super::firestore::DeleteDocumentRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::empty::Empty> {
        self.client.unary_call(&METHOD_FIRESTORE_DELETE_DOCUMENT, req, opt)
    }

    pub fn delete_document(&self, req: &super::firestore::DeleteDocumentRequest) -> ::grpcio::Result<super::empty::Empty> {
        self.delete_document_opt(req, ::grpcio::CallOption::default())
    }

    pub fn delete_document_async_opt(&self, req: &super::firestore::DeleteDocumentRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.client.unary_call_async(&METHOD_FIRESTORE_DELETE_DOCUMENT, req, opt)
    }

    pub fn delete_document_async(&self, req: &super::firestore::DeleteDocumentRequest) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.delete_document_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn batch_get_documents_opt(&self, req: &super::firestore::BatchGetDocumentsRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientSStreamReceiver<super::firestore::BatchGetDocumentsResponse>> {
        self.client.server_streaming(&METHOD_FIRESTORE_BATCH_GET_DOCUMENTS, req, opt)
    }

    pub fn batch_get_documents(&self, req: &super::firestore::BatchGetDocumentsRequest) -> ::grpcio::Result<::grpcio::ClientSStreamReceiver<super::firestore::BatchGetDocumentsResponse>> {
        self.batch_get_documents_opt(req, ::grpcio::CallOption::default())
    }

    pub fn begin_transaction_opt(&self, req: &super::firestore::BeginTransactionRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::firestore::BeginTransactionResponse> {
        self.client.unary_call(&METHOD_FIRESTORE_BEGIN_TRANSACTION, req, opt)
    }

    pub fn begin_transaction(&self, req: &super::firestore::BeginTransactionRequest) -> ::grpcio::Result<super::firestore::BeginTransactionResponse> {
        self.begin_transaction_opt(req, ::grpcio::CallOption::default())
    }

    pub fn begin_transaction_async_opt(&self, req: &super::firestore::BeginTransactionRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::firestore::BeginTransactionResponse>> {
        self.client.unary_call_async(&METHOD_FIRESTORE_BEGIN_TRANSACTION, req, opt)
    }

    pub fn begin_transaction_async(&self, req: &super::firestore::BeginTransactionRequest) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::firestore::BeginTransactionResponse>> {
        self.begin_transaction_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn commit_opt(&self, req: &super::firestore::CommitRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::firestore::CommitResponse> {
        self.client.unary_call(&METHOD_FIRESTORE_COMMIT, req, opt)
    }

    pub fn commit(&self, req: &super::firestore::CommitRequest) -> ::grpcio::Result<super::firestore::CommitResponse> {
        self.commit_opt(req, ::grpcio::CallOption::default())
    }

    pub fn commit_async_opt(&self, req: &super::firestore::CommitRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::firestore::CommitResponse>> {
        self.client.unary_call_async(&METHOD_FIRESTORE_COMMIT, req, opt)
    }

    pub fn commit_async(&self, req: &super::firestore::CommitRequest) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::firestore::CommitResponse>> {
        self.commit_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn rollback_opt(&self, req: &super::firestore::RollbackRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::empty::Empty> {
        self.client.unary_call(&METHOD_FIRESTORE_ROLLBACK, req, opt)
    }

    pub fn rollback(&self, req: &super::firestore::RollbackRequest) -> ::grpcio::Result<super::empty::Empty> {
        self.rollback_opt(req, ::grpcio::CallOption::default())
    }

    pub fn rollback_async_opt(&self, req: &super::firestore::RollbackRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.client.unary_call_async(&METHOD_FIRESTORE_ROLLBACK, req, opt)
    }

    pub fn rollback_async(&self, req: &super::firestore::RollbackRequest) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::empty::Empty>> {
        self.rollback_async_opt(req, ::grpcio::CallOption::default())
    }

    pub fn run_query_opt(&self, req: &super::firestore::RunQueryRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientSStreamReceiver<super::firestore::RunQueryResponse>> {
        self.client.server_streaming(&METHOD_FIRESTORE_RUN_QUERY, req, opt)
    }

    pub fn run_query(&self, req: &super::firestore::RunQueryRequest) -> ::grpcio::Result<::grpcio::ClientSStreamReceiver<super::firestore::RunQueryResponse>> {
        self.run_query_opt(req, ::grpcio::CallOption::default())
    }

    pub fn write_opt(&self, opt: ::grpcio::CallOption) -> ::grpcio::Result<(::grpcio::ClientDuplexSender<super::firestore::WriteRequest>, ::grpcio::ClientDuplexReceiver<super::firestore::WriteResponse>)> {
        self.client.duplex_streaming(&METHOD_FIRESTORE_WRITE, opt)
    }

    pub fn write(&self) -> ::grpcio::Result<(::grpcio::ClientDuplexSender<super::firestore::WriteRequest>, ::grpcio::ClientDuplexReceiver<super::firestore::WriteResponse>)> {
        self.write_opt(::grpcio::CallOption::default())
    }

    pub fn listen_opt(&self, opt: ::grpcio::CallOption) -> ::grpcio::Result<(::grpcio::ClientDuplexSender<super::firestore::ListenRequest>, ::grpcio::ClientDuplexReceiver<super::firestore::ListenResponse>)> {
        self.client.duplex_streaming(&METHOD_FIRESTORE_LISTEN, opt)
    }

    pub fn listen(&self) -> ::grpcio::Result<(::grpcio::ClientDuplexSender<super::firestore::ListenRequest>, ::grpcio::ClientDuplexReceiver<super::firestore::ListenResponse>)> {
        self.listen_opt(::grpcio::CallOption::default())
    }

    pub fn list_collection_ids_opt(&self, req: &super::firestore::ListCollectionIdsRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<super::firestore::ListCollectionIdsResponse> {
        self.client.unary_call(&METHOD_FIRESTORE_LIST_COLLECTION_IDS, req, opt)
    }

    pub fn list_collection_ids(&self, req: &super::firestore::ListCollectionIdsRequest) -> ::grpcio::Result<super::firestore::ListCollectionIdsResponse> {
        self.list_collection_ids_opt(req, ::grpcio::CallOption::default())
    }

    pub fn list_collection_ids_async_opt(&self, req: &super::firestore::ListCollectionIdsRequest, opt: ::grpcio::CallOption) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::firestore::ListCollectionIdsResponse>> {
        self.client.unary_call_async(&METHOD_FIRESTORE_LIST_COLLECTION_IDS, req, opt)
    }

    pub fn list_collection_ids_async(&self, req: &super::firestore::ListCollectionIdsRequest) -> ::grpcio::Result<::grpcio::ClientUnaryReceiver<super::firestore::ListCollectionIdsResponse>> {
        self.list_collection_ids_async_opt(req, ::grpcio::CallOption::default())
    }
    pub fn spawn<F>(&self, f: F) where F: ::futures::Future<Item = (), Error = ()> + Send + 'static {
        self.client.spawn(f)
    }
}

pub trait Firestore {
    fn get_document(&mut self, ctx: ::grpcio::RpcContext, req: super::firestore::GetDocumentRequest, sink: ::grpcio::UnarySink<super::document::Document>);
    fn list_documents(&mut self, ctx: ::grpcio::RpcContext, req: super::firestore::ListDocumentsRequest, sink: ::grpcio::UnarySink<super::firestore::ListDocumentsResponse>);
    fn create_document(&mut self, ctx: ::grpcio::RpcContext, req: super::firestore::CreateDocumentRequest, sink: ::grpcio::UnarySink<super::document::Document>);
    fn update_document(&mut self, ctx: ::grpcio::RpcContext, req: super::firestore::UpdateDocumentRequest, sink: ::grpcio::UnarySink<super::document::Document>);
    fn delete_document(&mut self, ctx: ::grpcio::RpcContext, req: super::firestore::DeleteDocumentRequest, sink: ::grpcio::UnarySink<super::empty::Empty>);
    fn batch_get_documents(&mut self, ctx: ::grpcio::RpcContext, req: super::firestore::BatchGetDocumentsRequest, sink: ::grpcio::ServerStreamingSink<super::firestore::BatchGetDocumentsResponse>);
    fn begin_transaction(&mut self, ctx: ::grpcio::RpcContext, req: super::firestore::BeginTransactionRequest, sink: ::grpcio::UnarySink<super::firestore::BeginTransactionResponse>);
    fn commit(&mut self, ctx: ::grpcio::RpcContext, req: super::firestore::CommitRequest, sink: ::grpcio::UnarySink<super::firestore::CommitResponse>);
    fn rollback(&mut self, ctx: ::grpcio::RpcContext, req: super::firestore::RollbackRequest, sink: ::grpcio::UnarySink<super::empty::Empty>);
    fn run_query(&mut self, ctx: ::grpcio::RpcContext, req: super::firestore::RunQueryRequest, sink: ::grpcio::ServerStreamingSink<super::firestore::RunQueryResponse>);
    fn write(&mut self, ctx: ::grpcio::RpcContext, stream: ::grpcio::RequestStream<super::firestore::WriteRequest>, sink: ::grpcio::DuplexSink<super::firestore::WriteResponse>);
    fn listen(&mut self, ctx: ::grpcio::RpcContext, stream: ::grpcio::RequestStream<super::firestore::ListenRequest>, sink: ::grpcio::DuplexSink<super::firestore::ListenResponse>);
    fn list_collection_ids(&mut self, ctx: ::grpcio::RpcContext, req: super::firestore::ListCollectionIdsRequest, sink: ::grpcio::UnarySink<super::firestore::ListCollectionIdsResponse>);
}

pub fn create_firestore<S: Firestore + Send + Clone + 'static>(s: S) -> ::grpcio::Service {
    let mut builder = ::grpcio::ServiceBuilder::new();
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_FIRESTORE_GET_DOCUMENT, move |ctx, req, resp| {
        instance.get_document(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_FIRESTORE_LIST_DOCUMENTS, move |ctx, req, resp| {
        instance.list_documents(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_FIRESTORE_CREATE_DOCUMENT, move |ctx, req, resp| {
        instance.create_document(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_FIRESTORE_UPDATE_DOCUMENT, move |ctx, req, resp| {
        instance.update_document(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_FIRESTORE_DELETE_DOCUMENT, move |ctx, req, resp| {
        instance.delete_document(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_server_streaming_handler(&METHOD_FIRESTORE_BATCH_GET_DOCUMENTS, move |ctx, req, resp| {
        instance.batch_get_documents(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_FIRESTORE_BEGIN_TRANSACTION, move |ctx, req, resp| {
        instance.begin_transaction(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_FIRESTORE_COMMIT, move |ctx, req, resp| {
        instance.commit(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_FIRESTORE_ROLLBACK, move |ctx, req, resp| {
        instance.rollback(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_server_streaming_handler(&METHOD_FIRESTORE_RUN_QUERY, move |ctx, req, resp| {
        instance.run_query(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_duplex_streaming_handler(&METHOD_FIRESTORE_WRITE, move |ctx, req, resp| {
        instance.write(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_duplex_streaming_handler(&METHOD_FIRESTORE_LISTEN, move |ctx, req, resp| {
        instance.listen(ctx, req, resp)
    });
    let mut instance = s.clone();
    builder = builder.add_unary_handler(&METHOD_FIRESTORE_LIST_COLLECTION_IDS, move |ctx, req, resp| {
        instance.list_collection_ids(ctx, req, resp)
    });
    builder.build()
}
