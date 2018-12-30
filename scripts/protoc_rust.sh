#!/bin/bash

/usr/local/bin/protoc --proto_path=.                  \
    --proto_path /opt/grpc/src/proto                  \
    --proto_path $HOME/opt/googleapis                 \
    --rust_out=tortuga/src/proto                      \
    --grpc_out=tortuga/src/proto                      \
    --plugin=protoc-gen-grpc=`which grpc_rust_plugin` \
    tortuga/tortuga.proto

/usr/local/bin/protoc --proto_path=.   \
    --proto_path /opt/grpc/src/proto   \
    --proto_path $HOME/opt/googleapis  \
    --rust_out=tortuga/src/proto       \
    tortuga/tortuga_params.proto
