package io.tortuga;

import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.common.util.concurrent.ListenableScheduledFuture;
import com.google.common.util.concurrent.ListeningScheduledExecutorService;
import com.google.common.util.concurrent.MoreExecutors;
import com.google.common.util.concurrent.SettableFuture;
import com.google.common.util.concurrent.ThreadFactoryBuilder;
import com.google.protobuf.Any;
import com.google.protobuf.StringValue;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.Status;
import io.grpc.Status.Code;
import io.grpc.StatusRuntimeException;
import io.tortuga.TortugaProto.CreateReq;
import io.tortuga.TortugaProto.CreateResp;
import io.tortuga.TortugaProto.TaskIdentifier;
import io.tortuga.TortugaProto.TaskProgress;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.TimeUnit;

public class TortugaConnection {
  private static final Logger LOG = LoggerFactory.getLogger(TortugaConnection.class);

  // Single thread that handles the heartbeat and the GRPC communication to server.
  private ListeningScheduledExecutorService maintenanceService;
  private final ManagedChannel chan;

  private final Map<String, SettableFuture<Status>> completionListeners = new ConcurrentHashMap<>();

  private ListenableScheduledFuture<?> heartbeatF;

  private TortugaConnection(ManagedChannel chan) {
    this.chan = chan;

    ThreadFactory tf = new ThreadFactoryBuilder()
        .setNameFormat("tortuga-internal-%d")
        .build();
    maintenanceService = MoreExecutors.listeningDecorator(Executors.newSingleThreadScheduledExecutor(tf));
  }

  public static TortugaConnection newConnection(String host, int port) {
    ManagedChannel chan = ManagedChannelBuilder.forAddress(host, port)
        .usePlaintext()
        .build();
    return new TortugaConnection(chan);
  }

  public synchronized void shutdown() {
    if (heartbeatF != null) {
      heartbeatF.cancel(false);
    }

    try {
      chan.shutdown();
      chan.awaitTermination(60L, TimeUnit.SECONDS);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      throw new IllegalStateException("interrupted while shuting down", ex);
    }

    maintenanceService.shutdown();
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

    if (spec.priority.isPresent()) {
      task.getPriorityBuilder().setValue(spec.priority.getAsInt());
    }

    if (spec.duration.isPresent()) {
      task.setDelay(spec.duration.get());
    }

    task.addAllModules(spec.modules);

    req.setTask(task);

    CreateResp resp = TortugaGrpc.newBlockingStub(chan)
        .withDeadlineAfter(10L, TimeUnit.SECONDS)
        .createTask(req.build());

    return new TaskResult(resp.getHandle(), resp.getCreated(), this);
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

    if (spec.priority.isPresent()) {
      task.getPriorityBuilder().setValue(spec.priority.getAsInt());
    }

    if (spec.duration.isPresent()) {
      task.setDelay(spec.duration.get());
    }

    task.addAllModules(spec.modules);

    req.setTask(task);

    ListenableFuture<CreateResp> respF = TortugaGrpc.newFutureStub(chan)
        .withDeadlineAfter(10L, TimeUnit.SECONDS)
        .createTask(req.build());
    return Futures.transform(respF, (resp) -> {
      return new TaskResult(resp.getHandle(), resp.getCreated(), this);
    });
  }

  public Tortuga newWorker(String workerId) {
    return new Tortuga(workerId, chan, maintenanceService);
  }

  boolean isDone(String handle) {
    return TortugaGrpc.newBlockingStub(chan)
        .withDeadlineAfter(30L, TimeUnit.SECONDS)
        .findTaskByHandle(StringValue.newBuilder()
            .setValue(handle)
            .build())
        .getDone();
  }

  TaskProgress getProgress(String handle) {
    return TortugaGrpc.newBlockingStub(chan)
        .withDeadlineAfter(30L, TimeUnit.SECONDS)
        .findTaskByHandle(StringValue.newBuilder()
            .setValue(handle)
            .build());
  }

  public Optional<TaskWatcher> createWatcher(String id, String type) {
    TaskIdentifier taskId = TaskIdentifier.newBuilder()
        .setId(id)
        .setType(type)
        .build();

    try {
      TaskProgress progress = TortugaGrpc.newBlockingStub(chan)
          .withDeadlineAfter(30L, TimeUnit.SECONDS)
          .findTask(taskId);

      return Optional.of(new TaskWatcher(progress.getHandle(), this));
    } catch (StatusRuntimeException ex) {
      if (ex.getStatus().getCode() == Code.NOT_FOUND) {
        return Optional.empty();
      }

      throw ex;
    }
  }

  /**
   * Returns a future for when a given task is complete.
   */
  public synchronized ListenableFuture<Status> completionFuture(String handle) {
    SettableFuture<Status> statusF = completionListeners.get(handle);
    if (statusF != null) {
      return statusF;
    }

    statusF = SettableFuture.create();
    completionListeners.put(handle, statusF);

    if (heartbeatF == null) {
      heartbeatF = maintenanceService.scheduleAtFixedRate(() -> {
        beatForCompletions();
      }, 100L, 100L, TimeUnit.MILLISECONDS);
    }

    return statusF;
  }

  private synchronized void beatForCompletions() {
    List<String> toRemove = new ArrayList<>();

    for (Map.Entry<String, SettableFuture<Status>> e : completionListeners.entrySet()) {
       try {
         TaskProgress progress = TortugaGrpc.newBlockingStub(chan)
             .withDeadlineAfter(5L, TimeUnit.SECONDS)
             .findTaskByHandle(StringValue.newBuilder().setValue(e.getKey()).build());

         if (progress.getDone()) {
           Status status = Status.fromCodeValue(progress.getStatus().getCode());
           e.getValue().set(status);
           toRemove.add(e.getKey());
         }
       } catch (StatusRuntimeException ex) {
         ex.printStackTrace();
       }
    }

    for (String key : toRemove) {
      completionListeners.remove(key);
    }
  }
}
