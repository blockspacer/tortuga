load("//:generate_workspace.bzl", "generated_java_libraries")

generated_java_libraries()

java_library(
    name = "tortuga_lib",
    srcs = glob(
        [
            "src/generated/java/**/*.java",
            "src/main/java/**/*.java",
        ],
    ),
    deps = [
        ":com_google_guava_guava",
        ":com_google_protobuf_protobuf_java",
        ":com_google_protobuf_protobuf_java_util",
        ":io_grpc_grpc_core",
        ":io_grpc_grpc_protobuf",
        ":io_grpc_grpc_stub",
        ":org_slf4j_slf4j_api",
    ],
)

java_test(
    name = "tortuga_test",
    srcs = [
        "src/test/java/io/tortuga/TortugaIntegrationTest.java",
    ],
    test_class = "io.tortuga.TortugaIntegrationTest",
    deps = [
        ":com_google_guava_guava",
        ":com_google_protobuf_protobuf_java",
        ":io_grpc_grpc_core",
        ":io_grpc_grpc_netty_shaded",
        ":org_slf4j_slf4j_api",
        ":tortuga_lib",
    ],
)
