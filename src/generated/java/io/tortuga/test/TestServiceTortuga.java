// This file is autogenerated by the Tortuga compiler, DO NOT EDIT MANUALLY

package io.tortuga.test;

import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;

import io.grpc.Status;
import io.tortuga.Service;
import io.tortuga.TaskHandlerRegistry;
import io.tortuga.TaskResult;
import io.tortuga.TaskSpec;
import io.tortuga.TortugaConnection;
import io.tortuga.TortugaContext;

public class TestServiceTortuga {
  public static class ImplBase extends Service {
    public ListenableFuture<Status> handleTask(com.google.protobuf.StringValue t, TortugaContext ctx) {
      return Futures.immediateFuture(Status.UNIMPLEMENTED);
    }

    public ListenableFuture<Status> handleCustomMessage(io.tortuga.test.TortugaProto.TestMessage t, TortugaContext ctx) {
      return Futures.immediateFuture(Status.UNIMPLEMENTED);
    }

    private ListenableFuture<Status> do_handleTaskImpl(com.google.protobuf.Any data, TortugaContext ctx) {
      try {
        com.google.protobuf.StringValue t = data.unpack(com.google.protobuf.StringValue.class);
        return handleTask(t, ctx);
      } catch (com.google.protobuf.InvalidProtocolBufferException ex) {
        Status status = Status.fromThrowable(ex);
        return Futures.immediateFuture(status);
      }
    }

    private ListenableFuture<Status> do_handleCustomMessageImpl(com.google.protobuf.Any data, TortugaContext ctx) {
      try {
        io.tortuga.test.TortugaProto.TestMessage t = data.unpack(io.tortuga.test.TortugaProto.TestMessage.class);
        return handleCustomMessage(t, ctx);
      } catch (com.google.protobuf.InvalidProtocolBufferException ex) {
        Status status = Status.fromThrowable(ex);
        return Futures.immediateFuture(status);
      }
    }


    @Override
    public final void register(TaskHandlerRegistry registry) {
      registry.registerHandler("tortuga.test.TestService.HandleTask", this::do_handleTaskImpl);
      registry.registerHandler("tortuga.test.TestService.HandleCustomMessage", this::do_handleCustomMessageImpl);
    }
  }
  
  public static final class Publisher {
    private final TortugaConnection conn;

    private Publisher(TortugaConnection conn) {
      this.conn = conn;
    }

    public TaskResult publishHandleTaskTask(TaskSpec spec, com.google.protobuf.StringValue t) {
      com.google.protobuf.Any data = com.google.protobuf.Any.pack(t);
      return conn.publishTask("tortuga.test.TestService.HandleTask", spec, data);
    }

    public TaskResult publishHandleCustomMessageTask(TaskSpec spec, io.tortuga.test.TortugaProto.TestMessage t) {
      com.google.protobuf.Any data = com.google.protobuf.Any.pack(t);
      return conn.publishTask("tortuga.test.TestService.HandleCustomMessage", spec, data);
    }

  }

  public static final class AsyncPublisher {
    private final TortugaConnection conn;

    private AsyncPublisher(TortugaConnection conn) {
      this.conn = conn;
    }

    public ListenableFuture<TaskResult> publishHandleTaskTask(TaskSpec spec, com.google.protobuf.StringValue t) {
      com.google.protobuf.Any data = com.google.protobuf.Any.pack(t);
      return conn.publishTaskAsync("tortuga.test.TestService.HandleTask", spec, data);
    }

    public ListenableFuture<TaskResult> publishHandleCustomMessageTask(TaskSpec spec, io.tortuga.test.TortugaProto.TestMessage t) {
      com.google.protobuf.Any data = com.google.protobuf.Any.pack(t);
      return conn.publishTaskAsync("tortuga.test.TestService.HandleCustomMessage", spec, data);
    }

  }

  public static Publisher newPublisher(TortugaConnection conn) {
    return new Publisher(conn);
  }

  public static AsyncPublisher newAsyncPublisher(TortugaConnection conn) {
    return new AsyncPublisher(conn);
  }
}
