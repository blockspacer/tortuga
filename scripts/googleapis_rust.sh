#!/bin/bash

generate_google() {
/usr/local/bin/protoc --proto_path=.      \
    --proto_path=$HOME/opt/googleapis     \
    --rust_out=$2                         \
    $1
}

generate_google google/api/annotations.proto google/src/api
generate_google google/api/http.proto google/src/api
generate_google google/rpc/status.proto google/src/rpc
generate_google google/type/latlng.proto google/src/type_
generate_google google/firestore/v1beta1/common.proto google/src/firestore
generate_google google/firestore/v1beta1/document.proto google/src/firestore
generate_google google/firestore/v1beta1/firestore.proto google/src/firestore
generate_google google/firestore/v1beta1/query.proto google/src/firestore
generate_google google/firestore/v1beta1/write.proto google/src/firestore
