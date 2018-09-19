#!/bin/bash

/usr/local/bin/protoc --proto_path=.  \
    --proto_path /opt/grpc/src/proto  \
    --proto_path $HOME/opt/googleapis \
    --grpc_out=.                      \
    --cpp_out=.                       \
    --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
    tortuga/tortuga.proto

/usr/local/bin/protoc --proto_path=.  \
    --proto_path /opt/grpc/src/proto  \
    --proto_path $HOME/opt/googleapis \
    --grpc_out=.                      \
    --cpp_out=.                       \
    --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
    tortuga/tortuga_params.proto

GRPC_JAVA_COMPILER=/opt/grpc-java/compiler/build/exe/java_plugin/protoc-gen-grpc-java

/usr/local/bin/protoc --java_out=src/main/java               \
    --proto_path .                                           \
    --proto_path $HOME/opt/googleapis                        \
    --java_plugin_out=src/main/java                          \
    --plugin=protoc-gen-java_plugin=$GRPC_JAVA_COMPILER      \
    tortuga/tortuga.proto

/usr/local/bin/protoc --java_out=src/main/java               \
    --proto_path .                                           \
    --proto_path $HOME/opt/googleapis                        \
    --java_plugin_out=src/main/java                          \
    --plugin=protoc-gen-java_plugin=$GRPC_JAVA_COMPILER      \
    tortuga/tortuga_params.proto