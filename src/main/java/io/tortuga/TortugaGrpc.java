package io.tortuga;

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
    comments = "Source: tortuga/tortuga.proto")
public final class TortugaGrpc {

  private TortugaGrpc() {}

  public static final String SERVICE_NAME = "tortuga.Tortuga";

  // Static method descriptors that strictly reflect the proto.
  private static volatile io.grpc.MethodDescriptor<io.tortuga.TortugaProto.CreateReq,
      io.tortuga.TortugaProto.CreateResp> getCreateTaskMethod;

  public static io.grpc.MethodDescriptor<io.tortuga.TortugaProto.CreateReq,
      io.tortuga.TortugaProto.CreateResp> getCreateTaskMethod() {
    io.grpc.MethodDescriptor<io.tortuga.TortugaProto.CreateReq, io.tortuga.TortugaProto.CreateResp> getCreateTaskMethod;
    if ((getCreateTaskMethod = TortugaGrpc.getCreateTaskMethod) == null) {
      synchronized (TortugaGrpc.class) {
        if ((getCreateTaskMethod = TortugaGrpc.getCreateTaskMethod) == null) {
          TortugaGrpc.getCreateTaskMethod = getCreateTaskMethod = 
              io.grpc.MethodDescriptor.<io.tortuga.TortugaProto.CreateReq, io.tortuga.TortugaProto.CreateResp>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "tortuga.Tortuga", "CreateTask"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.TortugaProto.CreateReq.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.TortugaProto.CreateResp.getDefaultInstance()))
                  .setSchemaDescriptor(new TortugaMethodDescriptorSupplier("CreateTask"))
                  .build();
          }
        }
     }
     return getCreateTaskMethod;
  }

  private static volatile io.grpc.MethodDescriptor<io.tortuga.TortugaProto.TaskReq,
      io.tortuga.TortugaProto.TaskResp> getRequestTaskMethod;

  public static io.grpc.MethodDescriptor<io.tortuga.TortugaProto.TaskReq,
      io.tortuga.TortugaProto.TaskResp> getRequestTaskMethod() {
    io.grpc.MethodDescriptor<io.tortuga.TortugaProto.TaskReq, io.tortuga.TortugaProto.TaskResp> getRequestTaskMethod;
    if ((getRequestTaskMethod = TortugaGrpc.getRequestTaskMethod) == null) {
      synchronized (TortugaGrpc.class) {
        if ((getRequestTaskMethod = TortugaGrpc.getRequestTaskMethod) == null) {
          TortugaGrpc.getRequestTaskMethod = getRequestTaskMethod = 
              io.grpc.MethodDescriptor.<io.tortuga.TortugaProto.TaskReq, io.tortuga.TortugaProto.TaskResp>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "tortuga.Tortuga", "RequestTask"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.TortugaProto.TaskReq.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.TortugaProto.TaskResp.getDefaultInstance()))
                  .setSchemaDescriptor(new TortugaMethodDescriptorSupplier("RequestTask"))
                  .build();
          }
        }
     }
     return getRequestTaskMethod;
  }

  private static volatile io.grpc.MethodDescriptor<io.tortuga.TortugaProto.HeartbeatReq,
      com.google.protobuf.Empty> getHeartbeatMethod;

  public static io.grpc.MethodDescriptor<io.tortuga.TortugaProto.HeartbeatReq,
      com.google.protobuf.Empty> getHeartbeatMethod() {
    io.grpc.MethodDescriptor<io.tortuga.TortugaProto.HeartbeatReq, com.google.protobuf.Empty> getHeartbeatMethod;
    if ((getHeartbeatMethod = TortugaGrpc.getHeartbeatMethod) == null) {
      synchronized (TortugaGrpc.class) {
        if ((getHeartbeatMethod = TortugaGrpc.getHeartbeatMethod) == null) {
          TortugaGrpc.getHeartbeatMethod = getHeartbeatMethod = 
              io.grpc.MethodDescriptor.<io.tortuga.TortugaProto.HeartbeatReq, com.google.protobuf.Empty>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "tortuga.Tortuga", "Heartbeat"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.TortugaProto.HeartbeatReq.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  com.google.protobuf.Empty.getDefaultInstance()))
                  .setSchemaDescriptor(new TortugaMethodDescriptorSupplier("Heartbeat"))
                  .build();
          }
        }
     }
     return getHeartbeatMethod;
  }

  private static volatile io.grpc.MethodDescriptor<io.tortuga.TortugaProto.CompleteTaskReq,
      com.google.protobuf.Empty> getCompleteTaskMethod;

  public static io.grpc.MethodDescriptor<io.tortuga.TortugaProto.CompleteTaskReq,
      com.google.protobuf.Empty> getCompleteTaskMethod() {
    io.grpc.MethodDescriptor<io.tortuga.TortugaProto.CompleteTaskReq, com.google.protobuf.Empty> getCompleteTaskMethod;
    if ((getCompleteTaskMethod = TortugaGrpc.getCompleteTaskMethod) == null) {
      synchronized (TortugaGrpc.class) {
        if ((getCompleteTaskMethod = TortugaGrpc.getCompleteTaskMethod) == null) {
          TortugaGrpc.getCompleteTaskMethod = getCompleteTaskMethod = 
              io.grpc.MethodDescriptor.<io.tortuga.TortugaProto.CompleteTaskReq, com.google.protobuf.Empty>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "tortuga.Tortuga", "CompleteTask"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.TortugaProto.CompleteTaskReq.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  com.google.protobuf.Empty.getDefaultInstance()))
                  .setSchemaDescriptor(new TortugaMethodDescriptorSupplier("CompleteTask"))
                  .build();
          }
        }
     }
     return getCompleteTaskMethod;
  }

  private static volatile io.grpc.MethodDescriptor<io.tortuga.TortugaProto.UpdateProgressReq,
      com.google.protobuf.Empty> getUpdateProgressMethod;

  public static io.grpc.MethodDescriptor<io.tortuga.TortugaProto.UpdateProgressReq,
      com.google.protobuf.Empty> getUpdateProgressMethod() {
    io.grpc.MethodDescriptor<io.tortuga.TortugaProto.UpdateProgressReq, com.google.protobuf.Empty> getUpdateProgressMethod;
    if ((getUpdateProgressMethod = TortugaGrpc.getUpdateProgressMethod) == null) {
      synchronized (TortugaGrpc.class) {
        if ((getUpdateProgressMethod = TortugaGrpc.getUpdateProgressMethod) == null) {
          TortugaGrpc.getUpdateProgressMethod = getUpdateProgressMethod = 
              io.grpc.MethodDescriptor.<io.tortuga.TortugaProto.UpdateProgressReq, com.google.protobuf.Empty>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "tortuga.Tortuga", "UpdateProgress"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.TortugaProto.UpdateProgressReq.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  com.google.protobuf.Empty.getDefaultInstance()))
                  .setSchemaDescriptor(new TortugaMethodDescriptorSupplier("UpdateProgress"))
                  .build();
          }
        }
     }
     return getUpdateProgressMethod;
  }

  private static volatile io.grpc.MethodDescriptor<io.tortuga.TortugaProto.TaskIdentifier,
      io.tortuga.TortugaProto.TaskProgress> getFindTaskMethod;

  public static io.grpc.MethodDescriptor<io.tortuga.TortugaProto.TaskIdentifier,
      io.tortuga.TortugaProto.TaskProgress> getFindTaskMethod() {
    io.grpc.MethodDescriptor<io.tortuga.TortugaProto.TaskIdentifier, io.tortuga.TortugaProto.TaskProgress> getFindTaskMethod;
    if ((getFindTaskMethod = TortugaGrpc.getFindTaskMethod) == null) {
      synchronized (TortugaGrpc.class) {
        if ((getFindTaskMethod = TortugaGrpc.getFindTaskMethod) == null) {
          TortugaGrpc.getFindTaskMethod = getFindTaskMethod = 
              io.grpc.MethodDescriptor.<io.tortuga.TortugaProto.TaskIdentifier, io.tortuga.TortugaProto.TaskProgress>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "tortuga.Tortuga", "FindTask"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.TortugaProto.TaskIdentifier.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.TortugaProto.TaskProgress.getDefaultInstance()))
                  .setSchemaDescriptor(new TortugaMethodDescriptorSupplier("FindTask"))
                  .build();
          }
        }
     }
     return getFindTaskMethod;
  }

  private static volatile io.grpc.MethodDescriptor<io.tortuga.TortugaProto.FindTaskReq,
      io.tortuga.TortugaProto.TaskProgress> getFindTaskByHandleMethod;

  public static io.grpc.MethodDescriptor<io.tortuga.TortugaProto.FindTaskReq,
      io.tortuga.TortugaProto.TaskProgress> getFindTaskByHandleMethod() {
    io.grpc.MethodDescriptor<io.tortuga.TortugaProto.FindTaskReq, io.tortuga.TortugaProto.TaskProgress> getFindTaskByHandleMethod;
    if ((getFindTaskByHandleMethod = TortugaGrpc.getFindTaskByHandleMethod) == null) {
      synchronized (TortugaGrpc.class) {
        if ((getFindTaskByHandleMethod = TortugaGrpc.getFindTaskByHandleMethod) == null) {
          TortugaGrpc.getFindTaskByHandleMethod = getFindTaskByHandleMethod = 
              io.grpc.MethodDescriptor.<io.tortuga.TortugaProto.FindTaskReq, io.tortuga.TortugaProto.TaskProgress>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "tortuga.Tortuga", "FindTaskByHandle"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.TortugaProto.FindTaskReq.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  io.tortuga.TortugaProto.TaskProgress.getDefaultInstance()))
                  .setSchemaDescriptor(new TortugaMethodDescriptorSupplier("FindTaskByHandle"))
                  .build();
          }
        }
     }
     return getFindTaskByHandleMethod;
  }

  private static volatile io.grpc.MethodDescriptor<com.google.protobuf.Empty,
      com.google.protobuf.Empty> getPingMethod;

  public static io.grpc.MethodDescriptor<com.google.protobuf.Empty,
      com.google.protobuf.Empty> getPingMethod() {
    io.grpc.MethodDescriptor<com.google.protobuf.Empty, com.google.protobuf.Empty> getPingMethod;
    if ((getPingMethod = TortugaGrpc.getPingMethod) == null) {
      synchronized (TortugaGrpc.class) {
        if ((getPingMethod = TortugaGrpc.getPingMethod) == null) {
          TortugaGrpc.getPingMethod = getPingMethod = 
              io.grpc.MethodDescriptor.<com.google.protobuf.Empty, com.google.protobuf.Empty>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "tortuga.Tortuga", "Ping"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  com.google.protobuf.Empty.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  com.google.protobuf.Empty.getDefaultInstance()))
                  .setSchemaDescriptor(new TortugaMethodDescriptorSupplier("Ping"))
                  .build();
          }
        }
     }
     return getPingMethod;
  }

  private static volatile io.grpc.MethodDescriptor<com.google.protobuf.Empty,
      com.google.protobuf.Empty> getQuitQuitQuitMethod;

  public static io.grpc.MethodDescriptor<com.google.protobuf.Empty,
      com.google.protobuf.Empty> getQuitQuitQuitMethod() {
    io.grpc.MethodDescriptor<com.google.protobuf.Empty, com.google.protobuf.Empty> getQuitQuitQuitMethod;
    if ((getQuitQuitQuitMethod = TortugaGrpc.getQuitQuitQuitMethod) == null) {
      synchronized (TortugaGrpc.class) {
        if ((getQuitQuitQuitMethod = TortugaGrpc.getQuitQuitQuitMethod) == null) {
          TortugaGrpc.getQuitQuitQuitMethod = getQuitQuitQuitMethod = 
              io.grpc.MethodDescriptor.<com.google.protobuf.Empty, com.google.protobuf.Empty>newBuilder()
              .setType(io.grpc.MethodDescriptor.MethodType.UNARY)
              .setFullMethodName(generateFullMethodName(
                  "tortuga.Tortuga", "QuitQuitQuit"))
              .setSampledToLocalTracing(true)
              .setRequestMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  com.google.protobuf.Empty.getDefaultInstance()))
              .setResponseMarshaller(io.grpc.protobuf.ProtoUtils.marshaller(
                  com.google.protobuf.Empty.getDefaultInstance()))
                  .setSchemaDescriptor(new TortugaMethodDescriptorSupplier("QuitQuitQuit"))
                  .build();
          }
        }
     }
     return getQuitQuitQuitMethod;
  }

  /**
   * Creates a new async stub that supports all call types for the service
   */
  public static TortugaStub newStub(io.grpc.Channel channel) {
    return new TortugaStub(channel);
  }

  /**
   * Creates a new blocking-style stub that supports unary and streaming output calls on the service
   */
  public static TortugaBlockingStub newBlockingStub(
      io.grpc.Channel channel) {
    return new TortugaBlockingStub(channel);
  }

  /**
   * Creates a new ListenableFuture-style stub that supports unary calls on the service
   */
  public static TortugaFutureStub newFutureStub(
      io.grpc.Channel channel) {
    return new TortugaFutureStub(channel);
  }

  /**
   */
  public static abstract class TortugaImplBase implements io.grpc.BindableService {

    /**
     */
    public void createTask(io.tortuga.TortugaProto.CreateReq request,
        io.grpc.stub.StreamObserver<io.tortuga.TortugaProto.CreateResp> responseObserver) {
      asyncUnimplementedUnaryCall(getCreateTaskMethod(), responseObserver);
    }

    /**
     */
    public void requestTask(io.tortuga.TortugaProto.TaskReq request,
        io.grpc.stub.StreamObserver<io.tortuga.TortugaProto.TaskResp> responseObserver) {
      asyncUnimplementedUnaryCall(getRequestTaskMethod(), responseObserver);
    }

    /**
     */
    public void heartbeat(io.tortuga.TortugaProto.HeartbeatReq request,
        io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
      asyncUnimplementedUnaryCall(getHeartbeatMethod(), responseObserver);
    }

    /**
     */
    public void completeTask(io.tortuga.TortugaProto.CompleteTaskReq request,
        io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
      asyncUnimplementedUnaryCall(getCompleteTaskMethod(), responseObserver);
    }

    /**
     */
    public void updateProgress(io.tortuga.TortugaProto.UpdateProgressReq request,
        io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
      asyncUnimplementedUnaryCall(getUpdateProgressMethod(), responseObserver);
    }

    /**
     * <pre>
     * Finds a task by id and type.
     * </pre>
     */
    public void findTask(io.tortuga.TortugaProto.TaskIdentifier request,
        io.grpc.stub.StreamObserver<io.tortuga.TortugaProto.TaskProgress> responseObserver) {
      asyncUnimplementedUnaryCall(getFindTaskMethod(), responseObserver);
    }

    /**
     * <pre>
     * Finds a task by specific handle.
     * </pre>
     */
    public void findTaskByHandle(io.tortuga.TortugaProto.FindTaskReq request,
        io.grpc.stub.StreamObserver<io.tortuga.TortugaProto.TaskProgress> responseObserver) {
      asyncUnimplementedUnaryCall(getFindTaskByHandleMethod(), responseObserver);
    }

    /**
     * <pre>
     * admin commands (for now these are for tests)
     * </pre>
     */
    public void ping(com.google.protobuf.Empty request,
        io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
      asyncUnimplementedUnaryCall(getPingMethod(), responseObserver);
    }

    /**
     */
    public void quitQuitQuit(com.google.protobuf.Empty request,
        io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
      asyncUnimplementedUnaryCall(getQuitQuitQuitMethod(), responseObserver);
    }

    @java.lang.Override public final io.grpc.ServerServiceDefinition bindService() {
      return io.grpc.ServerServiceDefinition.builder(getServiceDescriptor())
          .addMethod(
            getCreateTaskMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                io.tortuga.TortugaProto.CreateReq,
                io.tortuga.TortugaProto.CreateResp>(
                  this, METHODID_CREATE_TASK)))
          .addMethod(
            getRequestTaskMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                io.tortuga.TortugaProto.TaskReq,
                io.tortuga.TortugaProto.TaskResp>(
                  this, METHODID_REQUEST_TASK)))
          .addMethod(
            getHeartbeatMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                io.tortuga.TortugaProto.HeartbeatReq,
                com.google.protobuf.Empty>(
                  this, METHODID_HEARTBEAT)))
          .addMethod(
            getCompleteTaskMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                io.tortuga.TortugaProto.CompleteTaskReq,
                com.google.protobuf.Empty>(
                  this, METHODID_COMPLETE_TASK)))
          .addMethod(
            getUpdateProgressMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                io.tortuga.TortugaProto.UpdateProgressReq,
                com.google.protobuf.Empty>(
                  this, METHODID_UPDATE_PROGRESS)))
          .addMethod(
            getFindTaskMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                io.tortuga.TortugaProto.TaskIdentifier,
                io.tortuga.TortugaProto.TaskProgress>(
                  this, METHODID_FIND_TASK)))
          .addMethod(
            getFindTaskByHandleMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                io.tortuga.TortugaProto.FindTaskReq,
                io.tortuga.TortugaProto.TaskProgress>(
                  this, METHODID_FIND_TASK_BY_HANDLE)))
          .addMethod(
            getPingMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                com.google.protobuf.Empty,
                com.google.protobuf.Empty>(
                  this, METHODID_PING)))
          .addMethod(
            getQuitQuitQuitMethod(),
            asyncUnaryCall(
              new MethodHandlers<
                com.google.protobuf.Empty,
                com.google.protobuf.Empty>(
                  this, METHODID_QUIT_QUIT_QUIT)))
          .build();
    }
  }

  /**
   */
  public static final class TortugaStub extends io.grpc.stub.AbstractStub<TortugaStub> {
    private TortugaStub(io.grpc.Channel channel) {
      super(channel);
    }

    private TortugaStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected TortugaStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new TortugaStub(channel, callOptions);
    }

    /**
     */
    public void createTask(io.tortuga.TortugaProto.CreateReq request,
        io.grpc.stub.StreamObserver<io.tortuga.TortugaProto.CreateResp> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getCreateTaskMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void requestTask(io.tortuga.TortugaProto.TaskReq request,
        io.grpc.stub.StreamObserver<io.tortuga.TortugaProto.TaskResp> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getRequestTaskMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void heartbeat(io.tortuga.TortugaProto.HeartbeatReq request,
        io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getHeartbeatMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void completeTask(io.tortuga.TortugaProto.CompleteTaskReq request,
        io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getCompleteTaskMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void updateProgress(io.tortuga.TortugaProto.UpdateProgressReq request,
        io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getUpdateProgressMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     * <pre>
     * Finds a task by id and type.
     * </pre>
     */
    public void findTask(io.tortuga.TortugaProto.TaskIdentifier request,
        io.grpc.stub.StreamObserver<io.tortuga.TortugaProto.TaskProgress> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getFindTaskMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     * <pre>
     * Finds a task by specific handle.
     * </pre>
     */
    public void findTaskByHandle(io.tortuga.TortugaProto.FindTaskReq request,
        io.grpc.stub.StreamObserver<io.tortuga.TortugaProto.TaskProgress> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getFindTaskByHandleMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     * <pre>
     * admin commands (for now these are for tests)
     * </pre>
     */
    public void ping(com.google.protobuf.Empty request,
        io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getPingMethod(), getCallOptions()), request, responseObserver);
    }

    /**
     */
    public void quitQuitQuit(com.google.protobuf.Empty request,
        io.grpc.stub.StreamObserver<com.google.protobuf.Empty> responseObserver) {
      asyncUnaryCall(
          getChannel().newCall(getQuitQuitQuitMethod(), getCallOptions()), request, responseObserver);
    }
  }

  /**
   */
  public static final class TortugaBlockingStub extends io.grpc.stub.AbstractStub<TortugaBlockingStub> {
    private TortugaBlockingStub(io.grpc.Channel channel) {
      super(channel);
    }

    private TortugaBlockingStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected TortugaBlockingStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new TortugaBlockingStub(channel, callOptions);
    }

    /**
     */
    public io.tortuga.TortugaProto.CreateResp createTask(io.tortuga.TortugaProto.CreateReq request) {
      return blockingUnaryCall(
          getChannel(), getCreateTaskMethod(), getCallOptions(), request);
    }

    /**
     */
    public io.tortuga.TortugaProto.TaskResp requestTask(io.tortuga.TortugaProto.TaskReq request) {
      return blockingUnaryCall(
          getChannel(), getRequestTaskMethod(), getCallOptions(), request);
    }

    /**
     */
    public com.google.protobuf.Empty heartbeat(io.tortuga.TortugaProto.HeartbeatReq request) {
      return blockingUnaryCall(
          getChannel(), getHeartbeatMethod(), getCallOptions(), request);
    }

    /**
     */
    public com.google.protobuf.Empty completeTask(io.tortuga.TortugaProto.CompleteTaskReq request) {
      return blockingUnaryCall(
          getChannel(), getCompleteTaskMethod(), getCallOptions(), request);
    }

    /**
     */
    public com.google.protobuf.Empty updateProgress(io.tortuga.TortugaProto.UpdateProgressReq request) {
      return blockingUnaryCall(
          getChannel(), getUpdateProgressMethod(), getCallOptions(), request);
    }

    /**
     * <pre>
     * Finds a task by id and type.
     * </pre>
     */
    public io.tortuga.TortugaProto.TaskProgress findTask(io.tortuga.TortugaProto.TaskIdentifier request) {
      return blockingUnaryCall(
          getChannel(), getFindTaskMethod(), getCallOptions(), request);
    }

    /**
     * <pre>
     * Finds a task by specific handle.
     * </pre>
     */
    public io.tortuga.TortugaProto.TaskProgress findTaskByHandle(io.tortuga.TortugaProto.FindTaskReq request) {
      return blockingUnaryCall(
          getChannel(), getFindTaskByHandleMethod(), getCallOptions(), request);
    }

    /**
     * <pre>
     * admin commands (for now these are for tests)
     * </pre>
     */
    public com.google.protobuf.Empty ping(com.google.protobuf.Empty request) {
      return blockingUnaryCall(
          getChannel(), getPingMethod(), getCallOptions(), request);
    }

    /**
     */
    public com.google.protobuf.Empty quitQuitQuit(com.google.protobuf.Empty request) {
      return blockingUnaryCall(
          getChannel(), getQuitQuitQuitMethod(), getCallOptions(), request);
    }
  }

  /**
   */
  public static final class TortugaFutureStub extends io.grpc.stub.AbstractStub<TortugaFutureStub> {
    private TortugaFutureStub(io.grpc.Channel channel) {
      super(channel);
    }

    private TortugaFutureStub(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      super(channel, callOptions);
    }

    @java.lang.Override
    protected TortugaFutureStub build(io.grpc.Channel channel,
        io.grpc.CallOptions callOptions) {
      return new TortugaFutureStub(channel, callOptions);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<io.tortuga.TortugaProto.CreateResp> createTask(
        io.tortuga.TortugaProto.CreateReq request) {
      return futureUnaryCall(
          getChannel().newCall(getCreateTaskMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<io.tortuga.TortugaProto.TaskResp> requestTask(
        io.tortuga.TortugaProto.TaskReq request) {
      return futureUnaryCall(
          getChannel().newCall(getRequestTaskMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<com.google.protobuf.Empty> heartbeat(
        io.tortuga.TortugaProto.HeartbeatReq request) {
      return futureUnaryCall(
          getChannel().newCall(getHeartbeatMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<com.google.protobuf.Empty> completeTask(
        io.tortuga.TortugaProto.CompleteTaskReq request) {
      return futureUnaryCall(
          getChannel().newCall(getCompleteTaskMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<com.google.protobuf.Empty> updateProgress(
        io.tortuga.TortugaProto.UpdateProgressReq request) {
      return futureUnaryCall(
          getChannel().newCall(getUpdateProgressMethod(), getCallOptions()), request);
    }

    /**
     * <pre>
     * Finds a task by id and type.
     * </pre>
     */
    public com.google.common.util.concurrent.ListenableFuture<io.tortuga.TortugaProto.TaskProgress> findTask(
        io.tortuga.TortugaProto.TaskIdentifier request) {
      return futureUnaryCall(
          getChannel().newCall(getFindTaskMethod(), getCallOptions()), request);
    }

    /**
     * <pre>
     * Finds a task by specific handle.
     * </pre>
     */
    public com.google.common.util.concurrent.ListenableFuture<io.tortuga.TortugaProto.TaskProgress> findTaskByHandle(
        io.tortuga.TortugaProto.FindTaskReq request) {
      return futureUnaryCall(
          getChannel().newCall(getFindTaskByHandleMethod(), getCallOptions()), request);
    }

    /**
     * <pre>
     * admin commands (for now these are for tests)
     * </pre>
     */
    public com.google.common.util.concurrent.ListenableFuture<com.google.protobuf.Empty> ping(
        com.google.protobuf.Empty request) {
      return futureUnaryCall(
          getChannel().newCall(getPingMethod(), getCallOptions()), request);
    }

    /**
     */
    public com.google.common.util.concurrent.ListenableFuture<com.google.protobuf.Empty> quitQuitQuit(
        com.google.protobuf.Empty request) {
      return futureUnaryCall(
          getChannel().newCall(getQuitQuitQuitMethod(), getCallOptions()), request);
    }
  }

  private static final int METHODID_CREATE_TASK = 0;
  private static final int METHODID_REQUEST_TASK = 1;
  private static final int METHODID_HEARTBEAT = 2;
  private static final int METHODID_COMPLETE_TASK = 3;
  private static final int METHODID_UPDATE_PROGRESS = 4;
  private static final int METHODID_FIND_TASK = 5;
  private static final int METHODID_FIND_TASK_BY_HANDLE = 6;
  private static final int METHODID_PING = 7;
  private static final int METHODID_QUIT_QUIT_QUIT = 8;

  private static final class MethodHandlers<Req, Resp> implements
      io.grpc.stub.ServerCalls.UnaryMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.ServerStreamingMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.ClientStreamingMethod<Req, Resp>,
      io.grpc.stub.ServerCalls.BidiStreamingMethod<Req, Resp> {
    private final TortugaImplBase serviceImpl;
    private final int methodId;

    MethodHandlers(TortugaImplBase serviceImpl, int methodId) {
      this.serviceImpl = serviceImpl;
      this.methodId = methodId;
    }

    @java.lang.Override
    @java.lang.SuppressWarnings("unchecked")
    public void invoke(Req request, io.grpc.stub.StreamObserver<Resp> responseObserver) {
      switch (methodId) {
        case METHODID_CREATE_TASK:
          serviceImpl.createTask((io.tortuga.TortugaProto.CreateReq) request,
              (io.grpc.stub.StreamObserver<io.tortuga.TortugaProto.CreateResp>) responseObserver);
          break;
        case METHODID_REQUEST_TASK:
          serviceImpl.requestTask((io.tortuga.TortugaProto.TaskReq) request,
              (io.grpc.stub.StreamObserver<io.tortuga.TortugaProto.TaskResp>) responseObserver);
          break;
        case METHODID_HEARTBEAT:
          serviceImpl.heartbeat((io.tortuga.TortugaProto.HeartbeatReq) request,
              (io.grpc.stub.StreamObserver<com.google.protobuf.Empty>) responseObserver);
          break;
        case METHODID_COMPLETE_TASK:
          serviceImpl.completeTask((io.tortuga.TortugaProto.CompleteTaskReq) request,
              (io.grpc.stub.StreamObserver<com.google.protobuf.Empty>) responseObserver);
          break;
        case METHODID_UPDATE_PROGRESS:
          serviceImpl.updateProgress((io.tortuga.TortugaProto.UpdateProgressReq) request,
              (io.grpc.stub.StreamObserver<com.google.protobuf.Empty>) responseObserver);
          break;
        case METHODID_FIND_TASK:
          serviceImpl.findTask((io.tortuga.TortugaProto.TaskIdentifier) request,
              (io.grpc.stub.StreamObserver<io.tortuga.TortugaProto.TaskProgress>) responseObserver);
          break;
        case METHODID_FIND_TASK_BY_HANDLE:
          serviceImpl.findTaskByHandle((io.tortuga.TortugaProto.FindTaskReq) request,
              (io.grpc.stub.StreamObserver<io.tortuga.TortugaProto.TaskProgress>) responseObserver);
          break;
        case METHODID_PING:
          serviceImpl.ping((com.google.protobuf.Empty) request,
              (io.grpc.stub.StreamObserver<com.google.protobuf.Empty>) responseObserver);
          break;
        case METHODID_QUIT_QUIT_QUIT:
          serviceImpl.quitQuitQuit((com.google.protobuf.Empty) request,
              (io.grpc.stub.StreamObserver<com.google.protobuf.Empty>) responseObserver);
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

  private static abstract class TortugaBaseDescriptorSupplier
      implements io.grpc.protobuf.ProtoFileDescriptorSupplier, io.grpc.protobuf.ProtoServiceDescriptorSupplier {
    TortugaBaseDescriptorSupplier() {}

    @java.lang.Override
    public com.google.protobuf.Descriptors.FileDescriptor getFileDescriptor() {
      return io.tortuga.TortugaProto.getDescriptor();
    }

    @java.lang.Override
    public com.google.protobuf.Descriptors.ServiceDescriptor getServiceDescriptor() {
      return getFileDescriptor().findServiceByName("Tortuga");
    }
  }

  private static final class TortugaFileDescriptorSupplier
      extends TortugaBaseDescriptorSupplier {
    TortugaFileDescriptorSupplier() {}
  }

  private static final class TortugaMethodDescriptorSupplier
      extends TortugaBaseDescriptorSupplier
      implements io.grpc.protobuf.ProtoMethodDescriptorSupplier {
    private final String methodName;

    TortugaMethodDescriptorSupplier(String methodName) {
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
      synchronized (TortugaGrpc.class) {
        result = serviceDescriptor;
        if (result == null) {
          serviceDescriptor = result = io.grpc.ServiceDescriptor.newBuilder(SERVICE_NAME)
              .setSchemaDescriptor(new TortugaFileDescriptorSupplier())
              .addMethod(getCreateTaskMethod())
              .addMethod(getRequestTaskMethod())
              .addMethod(getHeartbeatMethod())
              .addMethod(getCompleteTaskMethod())
              .addMethod(getUpdateProgressMethod())
              .addMethod(getFindTaskMethod())
              .addMethod(getFindTaskByHandleMethod())
              .addMethod(getPingMethod())
              .addMethod(getQuitQuitQuitMethod())
              .build();
        }
      }
    }
    return result;
  }
}
