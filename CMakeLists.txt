cmake_minimum_required (VERSION 2.8)
project (tortuga)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions("-std=c++14")
add_definitions(-DGTEST_HAS_TR1_TUPLE=1)

# bringing gtest in your own tree and including it using cmake
# is the recommended way.
# see: third_party/gtest-1.7.0/README
add_subdirectory(third_party/googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


add_library(firestore_proto_lib
  google/firestore/v1beta1/common.grpc.pb.cc
  google/firestore/v1beta1/common.pb.cc
  google/firestore/v1beta1/document.grpc.pb.cc
  google/firestore/v1beta1/document.pb.cc
  google/firestore/v1beta1/firestore.grpc.pb.cc
  google/firestore/v1beta1/firestore.pb.cc
  google/firestore/v1beta1/query.grpc.pb.cc
  google/firestore/v1beta1/query.pb.cc
  google/firestore/v1beta1/write.grpc.pb.cc
  google/firestore/v1beta1/write.pb.cc
)

target_link_libraries(firestore_proto_lib
  grpc++
  gpr
  protobuf
)

add_library (tortuga_lib
  grpc/status/status.pb.cc
  grpc/status/status.grpc.pb.cc  
  sqlite3.c
  tortuga/baton_handler.cc
  tortuga/http.cc
  tortuga/modules/firestore.cc
  tortuga/module.cc
  tortuga/progress_manager.cc
  tortuga/rpc_opts.cc  
  tortuga/sqlite_statement.cc
  tortuga/time_logger.cc
  tortuga/time_utils.cc
  tortuga/tortuga.cc
  tortuga/tortuga.pb.cc
  tortuga/tortuga.grpc.pb.cc
)

target_link_libraries(tortuga_lib
  firestore_proto_lib
  boost_context
  boost_filesystem
  boost_thread
  boost_system
  dl
  glog
  gflags
  grpc++
  gpr
  folly
  protobuf
  proxygenlib
  pthread
  jemalloc
  snappy
  lz4
  bz2
  z
  crypto
  ssl
  wangle
)


add_executable(tortuga_main tortuga/tortuga_main.cc)

target_link_libraries (tortuga_main
  tortuga_lib
  folly
)

add_executable(http_client_main tortuga/http_client_main.cc)

target_link_libraries (http_client_main
  tortuga_lib
  folly
)

add_library (tortuga_plugin_lib
  tortuga/plugin/tortuga_plugin.cc
)

target_link_libraries(tortuga_plugin_lib
  boost_context
  boost_filesystem
  boost_thread
  boost_system
  dl
  glog
  gflags
  grpc++
  gpr
  folly
  protobuf
  pthread
  jemalloc
  snappy
  lz4
  bz2
  z
)

add_executable(protoc-gen-tortuga tortuga/plugin/protoc_gen_tortuga.cc)

target_link_libraries (protoc-gen-tortuga
  tortuga_lib
  folly
  protoc
  protobuf
)
