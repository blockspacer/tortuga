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

add_library (tortuga_lib
  sqlite3.c
  tortuga/baton_handler.cc
  tortuga/sqlite_statement.cc
  tortuga/time_logger.cc
  tortuga/tortuga.cc
  tortuga/tortuga.pb.cc
  tortuga/tortuga.grpc.pb.cc
)

target_link_libraries(tortuga_lib
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


add_executable(tortuga_main tortuga/tortuga_main.cc)

target_link_libraries (tortuga_main
  tortuga_lib
  folly
  follyinit
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
  follyinit
  protoc
  protobuf
)
