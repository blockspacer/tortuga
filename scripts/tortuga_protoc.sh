#!/bin/bash

make protoc-gen-tortuga
GLOG_logtostderr=1 protoc --plugin=protoc-gen-tortuga \
  --tortuga_out=src/generated/java                    \
  tortuga/test.proto

GRPC_JAVA_COMPILER=/opt/grpc-java/compiler/build/exe/java_plugin/protoc-gen-grpc-java
/usr/local/bin/protoc --proto_path=.                    \
    --java_out=src/generated/java                       \
    --java_plugin_out=src/generated/java                \
    --plugin=protoc-gen-java_plugin=$GRPC_JAVA_COMPILER \
    tortuga/test.proto
