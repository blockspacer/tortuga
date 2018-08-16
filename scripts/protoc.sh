#!/bin/bash

/usr/local/bin/protoc --proto_path=. \
    --grpc_out=.                     \
    --cpp_out=.                      \
    --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
    tortuga/tortuga.proto

GRPC_JAVA_COMPILER=/opt/grpc-java/compiler/build/exe/java_plugin/protoc-gen-grpc-java

/usr/local/bin/protoc --java_out=src/main/java                                          \
    --java_plugin_out=src/main/java                                                     \
    --plugin=protoc-gen-java_plugin=$GRPC_JAVA_COMPILER                                 \
    tortuga/tortuga.proto
