#!/bin/bash

/usr/local/bin/protoc --proto_path=. \
    --proto_path /opt/grpc/src/proto \
    --grpc_out=.                     \
    --cpp_out=.                      \
    --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
    tortuga/tortuga.proto

/usr/local/bin/protoc --proto_path=. \
    --proto_path /opt/grpc/src/proto \
    --cpp_out=.                      \
    grpc/status/status.proto

GRPC_JAVA_COMPILER=/opt/grpc-java/compiler/build/exe/java_plugin/protoc-gen-grpc-java

/usr/local/bin/protoc --java_out=src/main/java               \
    --proto_path .                                           \
    --proto_path /opt/grpc/src/proto                         \
    --java_plugin_out=src/main/java                          \
    --plugin=protoc-gen-java_plugin=$GRPC_JAVA_COMPILER      \
    tortuga/tortuga.proto

/usr/local/bin/protoc --java_out=src/main/java               \
    --proto_path .                                           \
    --proto_path /opt/grpc/src/proto                         \
    grpc/status/status.proto

generate_google() {
/usr/local/bin/protoc --proto_path=.                  \
    --proto_path third_party/googleapis               \
    --cpp_out=.                                       \
    --grpc_out=.                                      \
    --plugin=protoc-gen-grpc=`which grpc_cpp_plugin`  \
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
