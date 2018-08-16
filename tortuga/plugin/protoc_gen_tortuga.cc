#include <ctype.h>
#include <stdio.h> 
#include <set>
#include <sstream>
#include <string>

#include "boost/algorithm/string.hpp"
#include "folly/init/Init.h"
#include "folly/String.h"
#include "glog/logging.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/descriptor.pb.h"
#include "google/protobuf/compiler/plugin.pb.h"
#include "google/protobuf/compiler/java/java_names.h"

using google::protobuf::Descriptor;
using google::protobuf::DescriptorPool;
using google::protobuf::FileDescriptor;
using google::protobuf::FileDescriptorProto;
using google::protobuf::MethodDescriptor; 
using google::protobuf::ServiceDescriptor; 
using google::protobuf::compiler::CodeGeneratorRequest;
using google::protobuf::compiler::CodeGeneratorResponse;
using google::protobuf::compiler::CodeGeneratorResponse_File;

void GenerateService(const ServiceDescriptor* service,
                    CodeGeneratorResponse* resp);

int main(int argc, char** argv) {
  folly::init(&argc, &argv, true);
  
  LOG(INFO) << "running tortuga proto generator";
  CodeGeneratorRequest req;
  CodeGeneratorResponse resp;

  CHECK(req.ParseFromFileDescriptor(STDIN_FILENO));
  VLOG(3) << req.ShortDebugString();

  DescriptorPool pool;
  std::set<std::string> files_to_gen;

  for (const auto& file : req.proto_file()) {
    pool.BuildFile(file);
  }

  for (const auto& name : req.file_to_generate()) {
    files_to_gen.insert(name);
  }

  std::vector<const FileDescriptor*> res;
  for (const auto& file : req.proto_file()) {
    bool should_gen = files_to_gen.find(file.name()) != files_to_gen.end();
    if (!should_gen) {
      continue;
    }
    
    const FileDescriptor* file_desc = pool.FindFileByName(file.name());
    // we must have added it when building the pool.
    CHECK(file_desc != nullptr);
    LOG(INFO) << "generating for file: " << file.name();
    
    for (int i = 0; i < file_desc->service_count(); ++i) {
      const ServiceDescriptor* service = file_desc->service(i);
      GenerateService(service, &resp);
    }
  }

  CHECK(resp.SerializeToFileDescriptor(STDOUT_FILENO));
  LOG(INFO) << "we're all done, bye bye!";
}

std::string FullyQualifiedJava(const Descriptor* msg) {
  return google::protobuf::compiler::java::ClassName(msg);
}

const char* kDefaultPackage = "";
std::string FileJavaPackage(const FileDescriptor* file) {
  std::string result;

  if (file->options().has_java_package()) {
    result = file->options().java_package();
  } else {
    result = kDefaultPackage;
    if (!file->package().empty()) {
      if (!result.empty()) result += '.';
      result += file->package();
    }
  }

  return result;
}

std::string JavaMethodName(const MethodDescriptor* desc) {
  std::string method_name = desc->name();
  CHECK(!method_name.empty());
  method_name[0] = std::tolower(method_name[0]);
  return method_name;
}

void GenerateService(const ServiceDescriptor* service,
                     CodeGeneratorResponse* resp) {
  LOG(INFO) << "generating service: " << service->full_name();
  std::string pkg = FileJavaPackage(service->file());
  std::string klass = service->name() + "Tortuga";
  std::string written_file = pkg;
  boost::replace_all(written_file, ".", "/");
  written_file += ("/" + klass + ".java");

  std::ostringstream out;
  out << "// This file is autogenerated by the Tortuga compiler, DO NOT EDIT MANUALLY\n";
  out << "\n";
  out << "package " << pkg << ";\n";
  out << "\n";
  out << "import com.google.common.util.concurrent.Futures;\n";
  out << "import com.google.common.util.concurrent.ListenableFuture;\n";
  out << "\n";
  out << "import io.grpc.Status;\n";
  out << "import io.tortuga.Service;\n";
  out << "import io.tortuga.TaskHandlerRegistry;\n";
  out << "import io.tortuga.TaskResult;\n";
  out << "import io.tortuga.TaskSpec;\n";
  out << "import io.tortuga.TortugaConnection;\n";
  out << "import io.tortuga.TortugaContext;\n";
  out << "\n";
  out << "public class " << klass << " {\n";
  out << "  public static class ImplBase extends Service {\n";
  
  for (int i = 0; i < service->method_count(); ++i) {
    const MethodDescriptor* method = service->method(i);
    out << "    public ListenableFuture<Status> " << JavaMethodName(method) << "(" << FullyQualifiedJava(method->input_type()) << " t, TortugaContext ctx) {\n";
    out << "      return Futures.immediateFuture(Status.UNIMPLEMENTED);\n";
    out << "    }\n";
    out << "\n";
  }

  for (int i = 0; i < service->method_count(); ++i) {
    const MethodDescriptor* method = service->method(i);
    std::string t_type = FullyQualifiedJava(method->input_type());
    out << "    private ListenableFuture<Status> do_" << JavaMethodName(method) << "Impl(com.google.protobuf.Any data, TortugaContext ctx) {\n";
    out << "      try {\n";
    out << "        " << t_type << " t = data.unpack(" << t_type << ".class);\n";
    out << "        return " << JavaMethodName(method) << "(t, ctx);\n";
    out << "      } catch (com.google.protobuf.InvalidProtocolBufferException ex) {\n";
    out << "        Status status = Status.fromThrowable(ex);\n";
    out << "        return Futures.immediateFuture(status);\n";
    out << "      }\n";
    out << "    }\n";
    out << "\n";
  }

  out << "\n";
  out << "    @Override\n";
  out << "    public final void register(TaskHandlerRegistry registry) {\n";
  for (int i = 0; i < service->method_count(); ++i) {
    const MethodDescriptor* method = service->method(i);
    std::string t_type = FullyQualifiedJava(method->input_type());
    out << "      registry.registerHandler(\"" << method->full_name() << "\", this::do_" << JavaMethodName(method) << "Impl);\n";
  }  
  out << "    }\n";
  out << "  }\n";
  out << "  \n";
  out << "  public static final class Publisher {\n";
  out << "    private final TortugaConnection conn;\n";
  out << "\n";
  out << "    private Publisher(TortugaConnection conn) {\n";
  out << "      this.conn = conn;\n";
  out << "    }\n";
  out << "\n";

  for (int i = 0; i < service->method_count(); ++i) {
    const MethodDescriptor* method = service->method(i);
    out << "    public TaskResult publish" << method->name() << "Task(TaskSpec spec, " << FullyQualifiedJava(method->input_type()) << " t) {\n";
    out << "      com.google.protobuf.Any data = com.google.protobuf.Any.pack(t);\n";
    out << "      return conn.publishTask(\"" << method->full_name() << "\", spec, data);\n";
    out << "    }\n";
    out << "\n";
  }
  out << "  }\n";
  out << "\n";
  out << "  public static final class AsyncPublisher {\n";
  out << "    private final TortugaConnection conn;\n";
  out << "\n";
  out << "    private AsyncPublisher(TortugaConnection conn) {\n";
  out << "      this.conn = conn;\n";
  out << "    }\n";
  out << "\n";

  for (int i = 0; i < service->method_count(); ++i) {
    const MethodDescriptor* method = service->method(i);
    out << "    public ListenableFuture<TaskResult> publish" << method->name() << "Task(TaskSpec spec, " << FullyQualifiedJava(method->input_type()) << " t) {\n";
    out << "      com.google.protobuf.Any data = com.google.protobuf.Any.pack(t);\n";
    out << "      return conn.publishTaskAsync(\"" << method->full_name() << "\", spec, data);\n";
    out << "    }\n";
    out << "\n";
  }
  out << "  }\n";
  out << "\n";
  out << "  public static Publisher newPublisher(TortugaConnection conn) {\n";
  out << "    return new Publisher(conn);\n";
  out << "  }\n";
  out << "\n";
  out << "  public static AsyncPublisher newAsyncPublisher(TortugaConnection conn) {\n";
  out << "    return new AsyncPublisher(conn);\n";
  out << "  }\n";
  out << "}\n";

  CodeGeneratorResponse_File* file = resp->add_file();
  file->set_name(written_file);
  file->set_content(out.str());
}
