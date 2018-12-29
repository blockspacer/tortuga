package io.tortuga.test;

import static io.grpc.MethodDescriptor.generateFullMethodName;
import static io.grpc.stub.ClientCalls.asyncBidiStreamingCall;
import static io.grpc.stub.ClientCalls.asyncClientStreamingCall;
import static io.grpc.stub.ClientCalls.asyncServerStreamingCall;
import static io.grpc.stub.ClientCalls.asyncUnaryCall;
import static io.grpc.stub.ClientCalls.blockingServerStreamingCall;
import static io.grpc.stub.ClientCalls.blockingUnaryCall;
import static io.grpc.stub.ClientCalls.futureUnaryCall;
import static io.grpc.stub.ServerCalls.asyncBidiStreamingCall;
import static io.grpc.stub.ServerCalls.asyncClientStreamingCall;
import static io.grpc.stub.ServerCalls.asyncServerStreamingCall;
import static io.grpc.stub.ServerCalls.asyncUnaryCall;
import static io.grpc.stub.ServerCalls.asyncUnimplementedStreamingCall;
import static io.grpc.stub.ServerCalls.asyncUnimplementedUnaryCall;

/**
 */
@javax.annotation.Generated(
    value = "by gRPC proto compiler (version 1.13.1)",
    comments = "Source: tortuga/test.proto")
public final class TestService2Grpc {

  private TestService2Grpc() {}

  public static final String SERVICE_NAME = "tortuga.test.TestService2";

  // Static method descriptors that strictly reflect the proto.
  private static volatile io.grpc.MethodDescriptor<com.google.protobuf.StringValue,
      com.google.protobuf.Empty> getHandleTask2Method;

  public static io.grpc.MethodDescriptor<com.google.protobuf.StringValue,
      com.google.protobuf.Empty> getHandleTask2Method() {
    io.grpc.MethodDescriptor<com.google.protobuf.StringValue, com.google.protobuf.Empty> getHandleTask2Method;
    if ((getHandleTask2Method = TestService2Grpc.getHandleTask2Method) == null) {
      synchronized (TestService2Grpc.class) {
        if ((getHandleTask2Method = TestService2Grpc.getHandleTask2Method) == null) {
          TestService2Grpc.getHandleTask2Method = getHandleTask2Method = 
              io.grpc.MethodDescriptor.<com.google.protobuf.StringValue, com.google.protobuf.Empty>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "tortuga.test.TestService2", "HandleTask2"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  com.google.protobuf.StringValue.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  com.google.protobuf.Empty.getDefaultInstance()))
                  .setSchemaDescriptor(new TestService2MethodDescriptorSupplier("HandleTask2"))
                  .build();
          }
        }
     }
     return getHandleTask2Method;
  }

  private static volatile io.grpc.MethodDescriptor<io.tortuga.test.TortugaProto.TestMessage,
      io.tortuga.TortugaParamsProto.TortugaOutput> getHandleCustomMessage2Method;

  public static io.grpc.MethodDescriptor<io.tortuga.test.TortugaProto.TestMessage,
      io.tortuga.TortugaParamsProto.TortugaOutput> getHandleCustomMessage2Method() {
    io.grpc.MethodDescriptor<io.tortuga.test.TortugaProto.TestMessage, io.tortuga.TortugaParamsProto.TortugaOutput> getHandleCustomMessage2Method;
    if ((getHandleCustomMessage2Method = TestService2Grpc.getHandleCustomMessage2Method) == null) {
      synchronized (TestService2Grpc.class) {
        if ((getHandleCustomMessage2Method = TestService2Grpc.getHandleCustomMessage2Method) == null) {
          TestService2Grpc.getHandleCustomMessage2Method = getHandleCustomMessage2Method = 
              io.grpc.MethodDescriptor.<io.tortuga.test.TortugaProto.TestMessage, io.tortuga.TortugaParamsProto.TortugaOutput>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "tortuga.test.TestService2", "HandleCustomMessage2"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.test.TortugaProto.TestMessage.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.TortugaParamsProto.TortugaOutput.getDefaultInstance()))
                  .setSchemaDescriptor(new TestService2MethodDescriptorSupplier("HandleCustomMessage2"))
                  .build();
          }
        }
     }
     return getHandleCustomMessage2Method;
  }

  /**
   * Creates a new async stub that supports all call types for the service
   */
  public static TestService2Stub newStub(io.grpc.Channel channel) {
    return new TestService2Stub(channel);
  }

  /**
   * Creates a new blocking-style stub that supports unary and streaming output calls on the service
   */
  public static TestService2BlockingStub newBlockingStub(
      io.grpc.Channel channel) {
    return new TestService2BlockingStub(channel);
  }

  /**
   * Creates a new ListenableFuture-style stub that supports unary calls on the service
   */
  public static TestService2FutureStub newFutureStub(
      io.grpc.Channel channel) {
    return new TestService2FutureStub(channel);
  }

  /**
   */
  public static abstract class TestService2ImplBase implements io.grpc.BindableService {

    /**
     */
    public void handleTask2(com.google.protobuf.StringValue request,
        io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
      asyncUnimplementedUnaryCall(getHandleTask2Method(), responseObserver);
    }

    /**
     */
    public void handleCustomMessage2(io.tortuga.test.TortugaProto.TestMessage request,
        io.grpc.stub.StreamObserver<io.tortuga.TortugaParamsProto.TortugaOutput> responseObserver) {
      asyncUnimplementedUnaryCall(getHandleCustomMessage2Method(), responseObserver);
    }

    @java.lang.Override public final io.grpc.ServerServiceDefinition bindService() {
      return io.grpc.ServerServiceDefinition.builder(getServiceDescriptor())
          .addMethod(
            getHandleTask2Method(),
            asyncUnaryCall(
              new MethodHandlers<
                com.google.protobuf.StringValue,
                com.google.protobuf.Empty>(
                  this, METHODID_HANDLE_TASK2)))
          .addMethod(
            getHandleCustomMessage2Method(),
            asyncUnaryCall(
              new MethodHandlers<
                io.tortuga.test.TortugaProto.TestMessage,
                io.tortuga.TortugaParamsProto.TortugaOutput>(
                  this, METHODID_HANDLE_CUSTOM_MESSAGE2)))
          .build();
    }
  }

  /**
   */
  public static final class TestService2Stub extends io.grpc.stub.AbstractStub<TestService2Stub> {
    private TestService2Stub(io.grpc.Channel channel) {
      super(channel);
    }

    private TestService2Stub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected TestService2Stub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new TestService2Stub(channel, callOptions);
    }

    /**
     */
    public void handleTask2(com.google.protobuf.StringValue request,
        io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getHandleTask2Method(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void handleCustomMessage2(io.tortuga.test.TortugaProto.TestMessage request,
        io.grpc.stub.StreamObserver<io.tortuga.TortugaParamsProto.TortugaOutput> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getHandleCustomMessage2Method(), getCallOptions()), request, responseObserver);
    }
  }

  /**
   */
  public static final class TestService2BlockingStub extends io.grpc.stub.AbstractStub<TestService2BlockingStub> {
    private TestService2BlockingStub(io.grpc.Channel channel) {
      super(channel);
    }

    private TestService2BlockingStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected TestService2BlockingStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new TestService2BlockingStub(channel, callOptions);
    }

    /**
     */
    public com.google.protobuf.Empty handleTask2(com.google.protobuf.StringValue request) {
      return blockingUnaryCall(
          getChannel(), getHandleTask2Method(), getCallOptions(), request);
    }

    /**
     */
    public io.tortuga.TortugaParamsProto.TortugaOutput handleCustomMessage2(io.tortuga.test.TortugaProto.TestMessage request) {
      return blockingUnaryCall(
          getChannel(), getHandleCustomMessage2Method(), getCallOptions(), request);
    }
  }

  /**
   */
  public static final class TestService2FutureStub extends io.grpc.stub.AbstractStub<TestService2FutureStub> {
    private TestService2FutureStub(io.grpc.Channel channel) {
      super(channel);
    }

    private TestService2FutureStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected TestService2FutureStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new TestService2FutureStub(channel, callOptions);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<com.google.protobuf.Empty> handleTask2(
        com.google.protobuf.StringValue request) {
      return futureUnaryCall(
          getChannel().newCall(getHandleTask2Method(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<io.tortuga.TortugaParamsProto.TortugaOutput> handleCustomMessage2(
        io.tortuga.test.TortugaProto.TestMessage request) {
      return futureUnaryCall(
          getChannel().newCall(getHandleCustomMessage2Method(), getCallOptions()), request);
    }
  }

  private static final int METHODID_HANDLE_TASK2 = 0;
  private static final int METHODID_HANDLE_CUSTOM_MESSAGE2 = 1;

  private static final class MethodHandlers<Req, Resp> implements
      io.grpc.stub.ServerCalls.UnaryMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.ServerStreamingMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.ClientStreamingMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.BidiStreamingMethod<Req, Resp> {
    private final TestService2ImplBase serviceImpl;
    private final int methodId;

    MethodHandlers(TestService2ImplBase serviceImpl, int methodId) {
      this.serviceImpl = serviceImpl;
      this.methodId = methodId;
    }

    @java.lang.Override
    @java.lang.SuppressWarnings("unchecked")
    public void invoke(Req request, io.grpc.stub.StreamObserver<Resp> responseObserver) {
      switch (methodId) {
        case METHODID_HANDLE_TASK2:
          serviceImpl.handleTask2((com.google.protobuf.StringValue) request,
              (io.grpc.stub.StreamObserver<com.google.protobuf.Empty>) responseObserver);
          break;
        case METHODID_HANDLE_CUSTOM_MESSAGE2:
          serviceImpl.handleCustomMessage2((io.tortuga.test.TortugaProto.TestMessage) request,
              (io.grpc.stub.StreamObserver<io.tortuga.TortugaParamsProto.TortugaOutput>) responseObserver);
          break;
        default:
          throw new AssertionError();
      }
    }

    @java.lang.Override
    @java.lang.SuppressWarnings("unchecked")
    public io.grpc.stub.StreamObserver<Req> invoke(
        io.grpc.stub.StreamObserver<Resp> responseObserver) {
      switch (methodId) {
        default:
          throw new AssertionError();
      }
    }
  }

  private static abstract class TestService2BaseDescriptorSupplier
      implements io.grpc.protobuf.ProtoFileDescriptorSupplier, io.grpc.protobuf.ProtoServiceDescriptorSupplier {
    TestService2BaseDescriptorSupplier() {}

    @java.lang.Override
    public com.google.protobuf.Descriptors.FileDescriptor getFileDescriptor() {
      return io.tortuga.test.TortugaProto.getDescriptor();
    }

    @java.lang.Override
    public com.google.protobuf.Descriptors.ServiceDescriptor getServiceDescriptor() {
      return getFileDescriptor().findServiceByName("TestService2");
    }
  }

  private static final class TestService2FileDescriptorSupplier
      extends TestService2BaseDescriptorSupplier {
    TestService2FileDescriptorSupplier() {}
  }

  private static final class TestService2MethodDescriptorSupplier
      extends TestService2BaseDescriptorSupplier
      implements io.grpc.protobuf.ProtoMethodDescriptorSupplier {
    private final String methodName;

    TestService2MethodDescriptorSupplier(String methodName) {
      this.methodName = methodName;
    }

    @java.lang.Override
    public com.google.protobuf.Descriptors.MethodDescriptor getMethodDescriptor() {
      return getServiceDescriptor().findMethodByName(methodName);
    }
  }

  private static volatile io.grpc.ServiceDescriptor serviceDescriptor;

  public static io.grpc.ServiceDescriptor getServiceDescriptor() {
    io.grpc.ServiceDescriptor result = serviceDescriptor;
    if (result == null) {
      synchronized (TestService2Grpc.class) {
        result = serviceDescriptor;
        if (result == null) {
          serviceDescriptor = result = io.grpc.ServiceDescriptor.newBuilder(SERVICE_NAME)
              .setSchemaDescriptor(new TestService2FileDescriptorSupplier())
              .addMethod(getHandleTask2Method())
              .addMethod(getHandleCustomMessage2Method())
              .build();
        }
      }
    }
    return result;
  }
}
