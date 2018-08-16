#!/bin/bash

make protoc-gen-tortuga
GLOG_logtostderr=1 protoc --plugin=protoc-gen-tortuga \
  --tortuga_out=src/generated/java                    \
  tortuga/test.proto

/usr/local/bin/protoc --proto_path=.               \
    --java_out=src/generated/java                  \
    tortuga/test.proto