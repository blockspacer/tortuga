package io.tortuga;

import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.common.util.concurrent.ListenableScheduledFuture;
import com.google.common.util.concurrent.ListeningScheduledExecutorService;
import com.google.common.util.concurrent.MoreExecutors;
import com.google.common.util.concurrent.RateLimiter;
import com.google.common.util.concurrent.SettableFuture;
import com.google.common.util.concurrent.ThreadFactoryBuilder;
import com.google.protobuf.Any;
import com.google.protobuf.StringValue;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.Status;
import io.grpc.stub.StreamObserver;
import io.tortuga.TortugaProto.CreateReq;
import io.tortuga.TortugaProto.CreateResp;
import io.tortuga.TortugaProto.SubReq;
import io.tortuga.TortugaProto.SubResp;
import io.tortuga.TortugaProto.TaskProgress;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

public class TortugaConnection {
  private static final Logger LOG = LoggerFactory.getLogger(TortugaConnection.class);

  // Single thread that handles the heartbeat and the GRPC communication to server.
  private ListeningScheduledExecutorService maintenanceService;
  private final ManagedChannel chan;

  private final Map<String, SettableFuture<Status>> completionListeners = new ConcurrentHashMap<>();
  private StreamObserver<SubReq> subscribeStub;
  private AtomicBoolean lastInvalidator;

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

  public void shutdown() {
    if (subscribeStub != null) {
      subscribeStub.onCompleted();
    }
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

    if (spec.priority.isPresent()) {
      task.getPriorityBuilder().setValue(spec.priority.getAsInt());
    }

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

  /**
   * Returns a future for when a given task is complete.
   */
  public synchronized ListenableFuture<Status> completionFuture(String handle) {
    SettableFuture<Status> statusF = completionListeners.get(handle);
    if (statusF != null) {
      return statusF;
    }

    if (subscribeStub == null) {
      buildStub();
    }

    statusF = SettableFuture.create();
    completionListeners.put(handle, statusF);
    subscribeStub.onNext(SubReq.newBuilder().build().newBuilder().setHandle(handle).build());
    return statusF;
  }

  private synchronized void buildStub() {
    if (lastInvalidator != null) {
      subscribeStub.onCompleted();
      lastInvalidator.set(true);
    }

    AtomicBoolean invalidator = new AtomicBoolean();
    subscribeStub = TortugaGrpc.newStub(chan).progressSubscribe(new SubStreamObserver(invalidator));
    this.lastInvalidator = invalidator;

    if (heartbeatF == null) {
      heartbeatF = maintenanceService.scheduleAtFixedRate(() -> {
        this.subscribeStub.onNext(SubReq.newBuilder()
            .setIsBeat(true)
            .setNumberOfSubs(completionListeners.size())
            .build());
      }, 100L, 100L, TimeUnit.MILLISECONDS);
    }
  }

  private class SubStreamObserver implements StreamObserver<SubResp> {
    private final AtomicBoolean invalidated;
    SubStreamObserver(AtomicBoolean invalidated) {
      this.invalidated = invalidated;
    }

    @Override
    public void onNext(SubResp taskProgress) {
      if (!invalidated.get()) {
        onProgress(taskProgress);
      }
    }

    @Override
    public void onError(Throwable t) {
      onProgressError(t);
    }

    @Override
    public void onCompleted() {
      onProgressCompleted();
    }
  }

  private synchronized void onProgress(SubResp resp) {
    if (resp.getMustReconnect()) {
      reconnect();
      return;
    }

    TaskProgress progress = resp.getProgress();
    SettableFuture<Status> statusF = completionListeners.get(progress.getHandle());
    if (statusF == null) {
      // we are not listening anymore (ps: this is weird...);
    }

    Status status = Status.fromCodeValue(progress.getStatus().getCode())
        .withDescription(progress.getStatus().getMessage());
    completionListeners.remove(progress.getHandle());
    statusF.set(status);
  }

  private final RateLimiter logsErrors = RateLimiter.create(0.1);

  private void onProgressError(Throwable t) {
    if (logsErrors.tryAcquire()) {
      LOG.error("subscriber listener got an error, we will reconnect...", t);
      t.printStackTrace();
    }

    reconnect();
  }

  private void onProgressCompleted() {
    // Tortuga server shall never close streams absent of error.
    new AssertionError("unreachable").printStackTrace();
    reconnect();
  }

  private synchronized void reconnect() {
    buildStub();

    for (String handle : completionListeners.keySet()) {
      subscribeStub.onNext(SubReq.newBuilder().build().newBuilder().setHandle(handle).build());
    }
  }
}
