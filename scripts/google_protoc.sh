#!/bin/bash

generate_google() {
/usr/local/bin/protoc --proto_path=.                  \
    --proto_path=$HOME/opt/googleapis                 \
    --cpp_out=.                                       \
    --grpc_out=.                                      \
    --plugin=protoc-gen-grpc=`which grpc_cpp_plugin`  \
    $1

/usr/local/bin/protoc --proto_path=.                       \
    --proto_path=$HOME/opt/googleapis                      \
    --rust_out=src/googleapis                              \
    --grpc_out=src/googleapis                              \
    --plugin=protoc-gen-grpc=`which grpc_rust_plugin`      \
    $1
}

generate_google google/api/annotations.proto
generate_google google/api/http.proto
generate_google google/rpc/status.proto
generate_google google/type/latlng.proto
generate_google google/firestore/v1beta1/common.proto
generate_google google/firestore/v1beta1/document.proto
generate_google google/firestore/v1beta1/firestore.proto
generate_google google/firestore/v1beta1/query.proto
generate_google google/firestore/v1beta1/write.proto
