package io.tortuga;

import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.protobuf.Any;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.tortuga.TortugaProto.CreateReq;
import io.tortuga.TortugaProto.CreateResp;

import java.util.concurrent.TimeUnit;

public class TortugaConnection {
  private final ManagedChannel chan;

  private TortugaConnection(ManagedChannel chan) {
    this.chan = chan;
  }

  public static TortugaConnection newConnection(String host, int port) {
    ManagedChannel chan = ManagedChannelBuilder.forAddress(host, port)
        .usePlaintext()
        .build();
    return new TortugaConnection(chan);
  }

  public void shutdown() {
    try {
      chan.shutdown();
      chan.awaitTermination(60L, TimeUnit.SECONDS);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      throw new IllegalStateException("interrupted while shuting down", ex);
    }
  }

  public TaskResult publishTask(String type,
                                TaskSpec spec,
                                Any data) {
    CreateReq.Builder req = CreateReq.newBuilder();
    TortugaProto.Task.Builder task = TortugaProto.Task.newBuilder();
    task.setId(spec.id);
    task.setType(type);
    task.setData(data);

    if (spec.maxRetries.isPresent()) {
      task.getMaxRetriesBuilder().setValue(spec.maxRetries.getAsInt());
    }

    req.setTask(task);

    CreateResp resp = TortugaGrpc.newBlockingStub(chan)
        .withDeadlineAfter(10L, TimeUnit.SECONDS)
        .createTask(req.build());

    return new TaskResult(resp.getHandle(), resp.getCreated());
  }

  public ListenableFuture<TaskResult> publishTaskAsync(String type,
                                                       TaskSpec spec,
                                                       Any data) {
    CreateReq.Builder req = CreateReq.newBuilder();
    TortugaProto.Task.Builder task = TortugaProto.Task.newBuilder();
    task.setId(spec.id);
    task.setType(type);
    task.setData(data);

    if (spec.maxRetries.isPresent()) {
      task.getMaxRetriesBuilder().setValue(spec.maxRetries.getAsInt());
    }

    req.setTask(task);

    ListenableFuture<CreateResp> respF = TortugaGrpc.newFutureStub(chan)
        .withDeadlineAfter(10L, TimeUnit.SECONDS)
        .createTask(req.build());
    return Futures.transform(respF, (resp) -> {
      return new TaskResult(resp.getHandle(), resp.getCreated());
    });
  }

  public Tortuga newWorker(String workerId) {
    return new Tortuga(workerId, chan);
  }
}
