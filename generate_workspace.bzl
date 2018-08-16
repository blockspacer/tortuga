# The following dependencies were calculated from:
#
# generate_workspace -a com.google.api.grpc:proto-google-common-protos:1.0.0 -a io.grpc:grpc-alts:1.13.1 -a io.grpc:grpc-netty-shaded:1.13.1 -a io.grpc:grpc-protobuf:1.13.1 -a io.grpc:grpc-stub:1.13.1 -a javax.annotation:javax.annotation-api:1.2 -a com.google.protobuf:protobuf-java-util:3.5.1 -a org.slf4j:slf4j-api:1.7.25 -r http://central.maven.org/maven2/


def generated_maven_jars():
  # io.grpc:grpc-netty-shaded:jar:1.13.1 got requested version
  # io.grpc:grpc-grpclb:jar:1.13.1 got requested version
  # io.grpc:grpc-protobuf:jar:1.13.1 got requested version
  # io.grpc:grpc-stub:jar:1.13.1 got requested version
  # io.grpc:grpc-netty:jar:1.13.1 got requested version
  # io.grpc:grpc-protobuf-lite:jar:1.13.1 got requested version
  # io.grpc:grpc-alts:jar:1.13.1
  native.maven_jar(
      name = "io_grpc_grpc_core",
      artifact = "io.grpc:grpc-core:1.13.1",
      repository = "http://central.maven.org/maven2/",
      sha1 = "74f5300e1e4d5c2abc0e4fc927d24f4f593c5a7d",
  )


  # io.grpc:grpc-core:jar:1.13.1
  native.maven_jar(
      name = "com_google_code_findbugs_jsr305",
      artifact = "com.google.code.findbugs:jsr305:3.0.0",
      repository = "http://central.maven.org/maven2/",
      sha1 = "5871fb60dc68d67da54a663c3fd636a10a532948",
  )


  # io.grpc:grpc-protobuf:jar:1.13.1 got requested version
  # io.grpc:grpc-protobuf:jar:1.13.1
  native.maven_jar(
      name = "io_grpc_grpc_protobuf_lite",
      artifact = "io.grpc:grpc-protobuf-lite:1.13.1",
      repository = "http://central.maven.org/maven2/",
      sha1 = "6ca13585100ad99e1c6140a7c8138dd96596aaf5",
  )


  native.maven_jar(
      name = "javax_annotation_javax_annotation_api",
      artifact = "javax.annotation:javax.annotation-api:1.2",
      repository = "http://central.maven.org/maven2/",
      sha1 = "479c1e06db31c432330183f5cae684163f186146",
  )


  # io.netty:netty-handler-proxy:jar:4.1.25.Final got requested version
  # io.netty:netty-codec:jar:4.1.25.Final
  # io.netty:netty-handler:jar:4.1.25.Final got requested version
  native.maven_jar(
      name = "io_netty_netty_transport",
      artifact = "io.netty:netty-transport:4.1.25.Final",
      repository = "http://central.maven.org/maven2/",
      sha1 = "19a6f1f649894b6705aa9d8cbcced188dff133b0",
  )


  # io.grpc:grpc-netty:jar:1.13.1
  native.maven_jar(
      name = "io_netty_netty_handler_proxy",
      artifact = "io.netty:netty-handler-proxy:4.1.25.Final",
      repository = "http://central.maven.org/maven2/",
      sha1 = "be053c1e9f4ac5463883aa7b692798ac7841359a",
  )


  # com.google.api.grpc:proto-google-common-protos:jar:1.0.0
  # io.grpc:grpc-alts:jar:1.13.1 wanted version 3.5.1
  # io.grpc:grpc-protobuf:jar:1.13.1 wanted version 3.5.1
  # io.grpc:grpc-grpclb:jar:1.13.1 wanted version 3.5.1
  native.maven_jar(
      name = "com_google_protobuf_protobuf_java",
      artifact = "com.google.protobuf:protobuf-java:3.5.1",
      repository = "http://central.maven.org/maven2/",
  )


  native.maven_jar(
      name = "io_grpc_grpc_stub",
      artifact = "io.grpc:grpc-stub:1.13.1",
      repository = "http://central.maven.org/maven2/",
      sha1 = "2fc6e85e459ac522a3d81a78add8b6695f7525f1",
  )


  native.maven_jar(
      name = "io_grpc_grpc_protobuf",
      artifact = "io.grpc:grpc-protobuf:1.13.1",
      repository = "http://central.maven.org/maven2/",
      sha1 = "832f531a79249af2d9c1d50168301ba2bc45b54b",
  )


  # io.netty:netty-handler-proxy:jar:4.1.25.Final
  native.maven_jar(
      name = "io_netty_netty_codec_socks",
      artifact = "io.netty:netty-codec-socks:4.1.25.Final",
      repository = "http://central.maven.org/maven2/",
      sha1 = "81d672c2823d83825b4839673828bcf20fd53e2c",
  )


  # io.netty:netty-codec-socks:jar:4.1.25.Final got requested version
  # io.netty:netty-codec-http:jar:4.1.25.Final
  # io.netty:netty-handler:jar:4.1.25.Final got requested version
  native.maven_jar(
      name = "io_netty_netty_codec",
      artifact = "io.netty:netty-codec:4.1.25.Final",
      repository = "http://central.maven.org/maven2/",
      sha1 = "3e465c75bead40d06b5b9c0612b37cf77c548887",
  )


  # io.netty:netty-transport:jar:4.1.25.Final
  # io.netty:netty-handler:jar:4.1.25.Final got requested version
  native.maven_jar(
      name = "io_netty_netty_buffer",
      artifact = "io.netty:netty-buffer:4.1.25.Final",
      repository = "http://central.maven.org/maven2/",
      sha1 = "f366d0cc87b158ca064d27507127e3cc4eb2f089",
  )


  # io.grpc:grpc-alts:jar:1.13.1
  native.maven_jar(
      name = "io_grpc_grpc_netty",
      artifact = "io.grpc:grpc-netty:1.13.1",
      repository = "http://central.maven.org/maven2/",
      sha1 = "24d4b1accd3c5dfe77fdf2823e1a8fa5df36f98e",
  )


  # io.grpc:grpc-core:jar:1.13.1
  native.maven_jar(
      name = "com_google_errorprone_error_prone_annotations",
      artifact = "com.google.errorprone:error_prone_annotations:2.1.2",
      repository = "http://central.maven.org/maven2/",
      sha1 = "6dcc08f90f678ac33e5ef78c3c752b6f59e63e0c",
  )


  # io.netty:netty-transport:jar:4.1.25.Final
  native.maven_jar(
      name = "io_netty_netty_resolver",
      artifact = "io.netty:netty-resolver:4.1.25.Final",
      repository = "http://central.maven.org/maven2/",
      sha1 = "dc0965d00746b782b33f419b005cbc130973030d",
  )


  native.maven_jar(
      name = "com_google_protobuf_protobuf_java_util",
      artifact = "com.google.protobuf:protobuf-java-util:3.5.1",
      repository = "http://central.maven.org/maven2/",
      sha1 = "6e40a6a3f52455bd633aa2a0dba1a416e62b4575",
  )


  # io.grpc:grpc-core:jar:1.13.1
  native.maven_jar(
      name = "io_opencensus_opencensus_contrib_grpc_metrics",
      artifact = "io.opencensus:opencensus-contrib-grpc-metrics:0.12.3",
      repository = "http://central.maven.org/maven2/",
      sha1 = "a4c7ff238a91b901c8b459889b6d0d7a9d889b4d",
  )


  # io.opencensus:opencensus-contrib-grpc-metrics:jar:0.12.3 got requested version
  # io.grpc:grpc-core:jar:1.13.1
  native.maven_jar(
      name = "io_opencensus_opencensus_api",
      artifact = "io.opencensus:opencensus-api:0.12.3",
      repository = "http://central.maven.org/maven2/",
      sha1 = "743f074095f29aa985517299545e72cc99c87de0",
  )


  # io.netty:netty-buffer:jar:4.1.25.Final
  # io.netty:netty-resolver:jar:4.1.25.Final got requested version
  native.maven_jar(
      name = "io_netty_netty_common",
      artifact = "io.netty:netty-common:4.1.25.Final",
      repository = "http://central.maven.org/maven2/",
      sha1 = "e17d5c05c101fe14536ce3fb34b36c54e04791f6",
  )


  native.maven_jar(
      name = "io_grpc_grpc_netty_shaded",
      artifact = "io.grpc:grpc-netty-shaded:1.13.1",
      repository = "http://central.maven.org/maven2/",
      sha1 = "ccdc4f2c2791d93164c574fbfb90d614aa0849ae",
  )


  # io.netty:netty-handler-proxy:jar:4.1.25.Final got requested version
  # io.netty:netty-codec-http2:jar:4.1.25.Final
  native.maven_jar(
      name = "io_netty_netty_codec_http",
      artifact = "io.netty:netty-codec-http:4.1.25.Final",
      repository = "http://central.maven.org/maven2/",
      sha1 = "70888d3f2a829541378f68503ddd52c3193df35a",
  )


  native.maven_jar(
      name = "org_slf4j_slf4j_api",
      artifact = "org.slf4j:slf4j-api:1.7.25",
      repository = "http://central.maven.org/maven2/",
      sha1 = "da76ca59f6a57ee3102f8f9bd9cee742973efa8a",
  )


  # io.grpc:grpc-netty:jar:1.13.1
  native.maven_jar(
      name = "io_netty_netty_codec_http2",
      artifact = "io.netty:netty-codec-http2:4.1.25.Final",
      repository = "http://central.maven.org/maven2/",
      sha1 = "20ffe2d83900da019b69bc557bf211737b322f71",
  )


  # io.netty:netty-codec-http2:jar:4.1.25.Final
  native.maven_jar(
      name = "io_netty_netty_handler",
      artifact = "io.netty:netty-handler:4.1.25.Final",
      repository = "http://central.maven.org/maven2/",
      sha1 = "ecdfb8fe93a8b75db3ea8746d3437eed845c24bd",
  )


  # io.grpc:grpc-core:jar:1.13.1
  # com.google.protobuf:protobuf-java-util:bundle:3.3.0 got requested version
  native.maven_jar(
      name = "com_google_code_gson_gson",
      artifact = "com.google.code.gson:gson:2.7",
      repository = "http://central.maven.org/maven2/",
      sha1 = "751f548c85fa49f330cecbb1875893f971b33c4e",
  )


  # io.grpc:grpc-alts:jar:1.13.1
  native.maven_jar(
      name = "io_grpc_grpc_grpclb",
      artifact = "io.grpc:grpc-grpclb:1.13.1",
      repository = "http://central.maven.org/maven2/",
      sha1 = "6e098d28bd92bc003fe2d323732847824a701902",
  )


  # io.grpc:grpc-core:jar:1.13.1
  native.maven_jar(
      name = "io_grpc_grpc_context",
      artifact = "io.grpc:grpc-context:1.13.1",
      repository = "http://central.maven.org/maven2/",
      sha1 = "e8d8407217ffe43996700f667fa1f6bee0a8c19a",
  )


  native.maven_jar(
      name = "io_grpc_grpc_alts",
      artifact = "io.grpc:grpc-alts:1.13.1",
      repository = "http://central.maven.org/maven2/",
      sha1 = "7a171b835cd8d9830853f18e489414d09fddc2de",
  )


  # io.grpc:grpc-alts:jar:1.13.1
  native.maven_jar(
      name = "org_apache_commons_commons_lang3",
      artifact = "org.apache.commons:commons-lang3:3.5",
      repository = "http://central.maven.org/maven2/",
      sha1 = "6c6c702c89bfff3cd9e80b04d668c5e190d588c6",
  )


  # io.grpc:grpc-protobuf:jar:1.13.1 got requested version
  native.maven_jar(
      name = "com_google_api_grpc_proto_google_common_protos",
      artifact = "com.google.api.grpc:proto-google-common-protos:1.0.0",
      repository = "http://central.maven.org/maven2/",
      sha1 = "86f070507e28b930e50d218ee5b6788ef0dd05e6",
  )


  # io.grpc:grpc-protobuf:jar:1.13.1 got requested version
  # io.grpc:grpc-protobuf-lite:jar:1.13.1 got requested version
  # io.grpc:grpc-core:jar:1.13.1
  # com.google.protobuf:protobuf-java-util:bundle:3.3.0 got requested version
  native.maven_jar(
      name = "com_google_guava_guava",
      artifact = "com.google.guava:guava:20.0",
      repository = "http://central.maven.org/maven2/",
      sha1 = "89507701249388e1ed5ddcf8c41f4ce1be7831ef",
  )




def generated_java_libraries():
  native.java_library(
      name = "io_grpc_grpc_core",
      visibility = ["//visibility:public"],
      exports = ["@io_grpc_grpc_core//jar"],
      runtime_deps = [
          ":com_google_code_findbugs_jsr305",
          ":com_google_code_gson_gson",
          ":com_google_errorprone_error_prone_annotations",
          ":com_google_guava_guava",
          ":io_grpc_grpc_context",
          ":io_opencensus_opencensus_api",
          ":io_opencensus_opencensus_contrib_grpc_metrics",
      ],
  )


  native.java_library(
      name = "com_google_code_findbugs_jsr305",
      visibility = ["//visibility:public"],
      exports = ["@com_google_code_findbugs_jsr305//jar"],
  )


  native.java_library(
      name = "io_grpc_grpc_protobuf_lite",
      visibility = ["//visibility:public"],
      exports = ["@io_grpc_grpc_protobuf_lite//jar"],
      runtime_deps = [
          ":com_google_guava_guava",
          ":io_grpc_grpc_core",
      ],
  )


  native.java_library(
      name = "javax_annotation_javax_annotation_api",
      visibility = ["//visibility:public"],
      exports = ["@javax_annotation_javax_annotation_api//jar"],
  )


  native.java_library(
      name = "io_netty_netty_transport",
      visibility = ["//visibility:public"],
      exports = ["@io_netty_netty_transport//jar"],
      runtime_deps = [
          ":io_netty_netty_buffer",
          ":io_netty_netty_common",
          ":io_netty_netty_resolver",
      ],
  )


  native.java_library(
      name = "io_netty_netty_handler_proxy",
      visibility = ["//visibility:public"],
      exports = ["@io_netty_netty_handler_proxy//jar"],
      runtime_deps = [
          ":io_netty_netty_codec",
          ":io_netty_netty_codec_http",
          ":io_netty_netty_codec_socks",
          ":io_netty_netty_transport",
      ],
  )


  native.java_library(
      name = "com_google_protobuf_protobuf_java",
      visibility = ["//visibility:public"],
      exports = ["@com_google_protobuf_protobuf_java//jar"],
  )


  native.java_library(
      name = "io_grpc_grpc_stub",
      visibility = ["//visibility:public"],
      exports = ["@io_grpc_grpc_stub//jar"],
      runtime_deps = [
          ":io_grpc_grpc_core",
      ],
  )


  native.java_library(
      name = "io_grpc_grpc_protobuf",
      visibility = ["//visibility:public"],
      exports = ["@io_grpc_grpc_protobuf//jar"],
      runtime_deps = [
          ":com_google_api_grpc_proto_google_common_protos",
          ":com_google_guava_guava",
          ":com_google_protobuf_protobuf_java",
          ":io_grpc_grpc_core",
          ":io_grpc_grpc_protobuf_lite",
      ],
  )


  native.java_library(
      name = "io_netty_netty_codec_socks",
      visibility = ["//visibility:public"],
      exports = ["@io_netty_netty_codec_socks//jar"],
      runtime_deps = [
          ":io_netty_netty_codec",
      ],
  )


  native.java_library(
      name = "io_netty_netty_codec",
      visibility = ["//visibility:public"],
      exports = ["@io_netty_netty_codec//jar"],
      runtime_deps = [
          ":io_netty_netty_buffer",
          ":io_netty_netty_common",
          ":io_netty_netty_resolver",
          ":io_netty_netty_transport",
      ],
  )


  native.java_library(
      name = "io_netty_netty_buffer",
      visibility = ["//visibility:public"],
      exports = ["@io_netty_netty_buffer//jar"],
      runtime_deps = [
          ":io_netty_netty_common",
      ],
  )


  native.java_library(
      name = "io_grpc_grpc_netty",
      visibility = ["//visibility:public"],
      exports = ["@io_grpc_grpc_netty//jar"],
      runtime_deps = [
          ":io_grpc_grpc_core",
          ":io_netty_netty_buffer",
          ":io_netty_netty_codec",
          ":io_netty_netty_codec_http",
          ":io_netty_netty_codec_http2",
          ":io_netty_netty_codec_socks",
          ":io_netty_netty_common",
          ":io_netty_netty_handler",
          ":io_netty_netty_handler_proxy",
          ":io_netty_netty_resolver",
          ":io_netty_netty_transport",
      ],
  )


  native.java_library(
      name = "com_google_errorprone_error_prone_annotations",
      visibility = ["//visibility:public"],
      exports = ["@com_google_errorprone_error_prone_annotations//jar"],
  )


  native.java_library(
      name = "io_netty_netty_resolver",
      visibility = ["//visibility:public"],
      exports = ["@io_netty_netty_resolver//jar"],
      runtime_deps = [
          ":io_netty_netty_common",
      ],
  )


  native.java_library(
      name = "com_google_protobuf_protobuf_java_util",
      visibility = ["//visibility:public"],
      exports = ["@com_google_protobuf_protobuf_java_util//jar"],
      runtime_deps = [
          ":com_google_code_gson_gson",
          ":com_google_guava_guava",
          ":com_google_protobuf_protobuf_java",
      ],
  )


  native.java_library(
      name = "io_opencensus_opencensus_contrib_grpc_metrics",
      visibility = ["//visibility:public"],
      exports = ["@io_opencensus_opencensus_contrib_grpc_metrics//jar"],
      runtime_deps = [
          ":io_opencensus_opencensus_api",
      ],
  )


  native.java_library(
      name = "io_opencensus_opencensus_api",
      visibility = ["//visibility:public"],
      exports = ["@io_opencensus_opencensus_api//jar"],
  )


  native.java_library(
      name = "io_netty_netty_common",
      visibility = ["//visibility:public"],
      exports = ["@io_netty_netty_common//jar"],
  )


  native.java_library(
      name = "io_grpc_grpc_netty_shaded",
      visibility = ["//visibility:public"],
      exports = ["@io_grpc_grpc_netty_shaded//jar"],
      runtime_deps = [
          ":io_grpc_grpc_core",
      ],
  )


  native.java_library(
      name = "io_netty_netty_codec_http",
      visibility = ["//visibility:public"],
      exports = ["@io_netty_netty_codec_http//jar"],
      runtime_deps = [
          ":io_netty_netty_buffer",
          ":io_netty_netty_codec",
          ":io_netty_netty_common",
          ":io_netty_netty_resolver",
          ":io_netty_netty_transport",
      ],
  )


  native.java_library(
      name = "org_slf4j_slf4j_api",
      visibility = ["//visibility:public"],
      exports = ["@org_slf4j_slf4j_api//jar"],
  )


  native.java_library(
      name = "io_netty_netty_codec_http2",
      visibility = ["//visibility:public"],
      exports = ["@io_netty_netty_codec_http2//jar"],
      runtime_deps = [
          ":io_netty_netty_buffer",
          ":io_netty_netty_codec",
          ":io_netty_netty_codec_http",
          ":io_netty_netty_common",
          ":io_netty_netty_handler",
          ":io_netty_netty_resolver",
          ":io_netty_netty_transport",
      ],
  )


  native.java_library(
      name = "io_netty_netty_handler",
      visibility = ["//visibility:public"],
      exports = ["@io_netty_netty_handler//jar"],
      runtime_deps = [
          ":io_netty_netty_buffer",
          ":io_netty_netty_codec",
          ":io_netty_netty_transport",
      ],
  )


  native.java_library(
      name = "com_google_code_gson_gson",
      visibility = ["//visibility:public"],
      exports = ["@com_google_code_gson_gson//jar"],
  )


  native.java_library(
      name = "io_grpc_grpc_grpclb",
      visibility = ["//visibility:public"],
      exports = ["@io_grpc_grpc_grpclb//jar"],
      runtime_deps = [
          ":com_google_code_gson_gson",
          ":com_google_protobuf_protobuf_java",
          ":com_google_protobuf_protobuf_java_util",
          ":io_grpc_grpc_core",
          ":io_grpc_grpc_protobuf",
          ":io_grpc_grpc_stub",
      ],
  )


  native.java_library(
      name = "io_grpc_grpc_context",
      visibility = ["//visibility:public"],
      exports = ["@io_grpc_grpc_context//jar"],
  )


  native.java_library(
      name = "io_grpc_grpc_alts",
      visibility = ["//visibility:public"],
      exports = ["@io_grpc_grpc_alts//jar"],
      runtime_deps = [
          ":com_google_api_grpc_proto_google_common_protos",
          ":com_google_code_findbugs_jsr305",
          ":com_google_code_gson_gson",
          ":com_google_errorprone_error_prone_annotations",
          ":com_google_guava_guava",
          ":com_google_protobuf_protobuf_java",
          ":com_google_protobuf_protobuf_java_util",
          ":io_grpc_grpc_context",
          ":io_grpc_grpc_core",
          ":io_grpc_grpc_grpclb",
          ":io_grpc_grpc_netty",
          ":io_grpc_grpc_protobuf",
          ":io_grpc_grpc_protobuf_lite",
          ":io_grpc_grpc_stub",
          ":io_netty_netty_buffer",
          ":io_netty_netty_codec",
          ":io_netty_netty_codec_http",
          ":io_netty_netty_codec_http2",
          ":io_netty_netty_codec_socks",
          ":io_netty_netty_common",
          ":io_netty_netty_handler",
          ":io_netty_netty_handler_proxy",
          ":io_netty_netty_resolver",
          ":io_netty_netty_transport",
          ":io_opencensus_opencensus_api",
          ":io_opencensus_opencensus_contrib_grpc_metrics",
          ":org_apache_commons_commons_lang3",
      ],
  )


  native.java_library(
      name = "org_apache_commons_commons_lang3",
      visibility = ["//visibility:public"],
      exports = ["@org_apache_commons_commons_lang3//jar"],
  )


  native.java_library(
      name = "com_google_api_grpc_proto_google_common_protos",
      visibility = ["//visibility:public"],
      exports = ["@com_google_api_grpc_proto_google_common_protos//jar"],
      runtime_deps = [
          ":com_google_protobuf_protobuf_java",
      ],
  )


  native.java_library(
      name = "com_google_guava_guava",
      visibility = ["//visibility:public"],
      exports = ["@com_google_guava_guava//jar"],
  )


