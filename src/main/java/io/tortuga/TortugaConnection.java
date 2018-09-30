package io.tortuga;

import com.google.common.collect.ImmutableList;
import com.google.common.collect.ImmutableMap;
import com.google.common.util.concurrent.FutureCallback;
import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.common.util.concurrent.ListenableScheduledFuture;
import com.google.common.util.concurrent.ListeningScheduledExecutorService;
import com.google.common.util.concurrent.MoreExecutors;
import com.google.common.util.concurrent.SettableFuture;
import com.google.common.util.concurrent.ThreadFactoryBuilder;
import com.google.protobuf.Any;
import com.google.protobuf.Empty;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.Status;
import io.grpc.Status.Code;
import io.grpc.StatusRuntimeException;
import io.tortuga.TortugaProto.CreateReq;
import io.tortuga.TortugaProto.CreateResp;
import io.tortuga.TortugaProto.FindTaskReq;
import io.tortuga.TortugaProto.HeartbeatReq;
import io.tortuga.TortugaProto.TaskIdentifier;
import io.tortuga.TortugaProto.TaskProgress;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.concurrent.Executors;
import java.util.concurrent.Semaphore;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.TimeUnit;

public class TortugaConnection {
  private static final Logger LOG = LoggerFactory.getLogger(TortugaConnection.class);

  // Single thread that handles the heartbeat and the GRPC communication to server.
  private ListeningScheduledExecutorService maintenanceService;
  private final ManagedChannel chan;

  private static final class TaskCompletionListener {
    // The future to set when the task completed.
    final SettableFuture<Status> future = SettableFuture.create();
    // Such that there is only one beating at a time for the completion of this task.
    final Semaphore beatingSem = new Semaphore(1);
  }

  private final Map<String, TaskCompletionListener> completionListeners = new HashMap<>();
  private ListenableScheduledFuture<?> completionBeatF;

  // There is never the need for more than a heartbeat at once...
  private final Semaphore heartbeatSem = new Semaphore(1);
  private final List<Tortuga> startedWorkers = new ArrayList<>();
  private ListenableScheduledFuture<?> heartBeatF;

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
    if (completionBeatF != null) {
      completionBeatF.cancel(false);
    }

    if (heartBeatF != null) {
      heartBeatF.cancel(false);
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
    Tortuga tortuga = new Tortuga(workerId, chan, this, maintenanceService);
    return tortuga;
  }

  void workerIsStarted(Tortuga tortuga) {
    synchronized (startedWorkers) {
      startedWorkers.add(tortuga);
    }

    if (heartBeatF == null) {
      heartBeatF = maintenanceService.scheduleWithFixedDelay(() -> {
        heartbeatForAllWorkers();
      }, 1L, 1L, TimeUnit.SECONDS);
    }
  }

  void workerShutdown(Tortuga tortuga) {
    synchronized (startedWorkers) {
      startedWorkers.remove(tortuga);
    }
  }

  boolean isDone(String handle) {
    FindTaskReq req = FindTaskReq.newBuilder()
        .setHandle(Long.parseLong(handle))
        .setIsForDoneOnly(true)
        .build();

    return TortugaGrpc.newBlockingStub(chan)
        .withDeadlineAfter(30L, TimeUnit.SECONDS)
        .findTaskByHandle(req)
        .getDone();
  }

  TaskProgress getProgress(String handle) {
    FindTaskReq req = FindTaskReq.newBuilder()
        .setHandle(Long.parseLong(handle))
        .setIsForDoneOnly(false)
        .build();

    return TortugaGrpc.newBlockingStub(chan)
        .withDeadlineAfter(30L, TimeUnit.SECONDS)
        .findTaskByHandle(req);
  }

  ListenableFuture<TaskProgress> getProgressAsync(String handle) {
    FindTaskReq req = FindTaskReq.newBuilder()
        .setHandle(Long.parseLong(handle))
        .setIsForDoneOnly(false)
        .build();

    return TortugaGrpc.newFutureStub(chan)
        .withDeadlineAfter(30L, TimeUnit.SECONDS)
        .findTaskByHandle(req);
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

      return Optional.of(new TaskWatcher(progress, this));
    } catch (StatusRuntimeException ex) {
      if (ex.getStatus().getCode() == Code.NOT_FOUND) {
        return Optional.empty();
      }

      throw ex;
    }
  }

  public ListenableFuture<Optional<TaskWatcher>> createWatcherAsync(String id, String type) {
    TaskIdentifier taskId = TaskIdentifier.newBuilder()
        .setId(id)
        .setType(type)
        .build();

    ListenableFuture<TaskProgress> taskF = TortugaGrpc.newFutureStub(chan)
        .withDeadlineAfter(30L, TimeUnit.SECONDS)
        .findTask(taskId);

    ListenableFuture<Optional<TaskWatcher>> watcherF = Futures.transform(taskF, progress -> {
      return Optional.of(new TaskWatcher(progress, this));
    });

    watcherF = Futures.catching(watcherF, StatusRuntimeException.class, (ex) -> {
      if (ex.getStatus().getCode() == Code.NOT_FOUND) {
        return Optional.empty();
      } else {
        throw ex;
      }
    });

    return watcherF;
  }

  /**
   * Returns a future for when a given task is complete.
   */
  public synchronized ListenableFuture<Status> completionFuture(String handle) {
    TaskCompletionListener listener = null;

    synchronized (completionListeners) {
      listener = completionListeners.get(handle);
      if (listener != null) {
        return listener.future;
      }

      listener = new TaskCompletionListener();
      completionListeners.put(handle, listener);
    }

    synchronized (this) {
      if (completionBeatF == null) {
        completionBeatF = maintenanceService.scheduleWithFixedDelay(() -> {
          beatForCompletions();
        }, 100L, 100L, TimeUnit.MILLISECONDS);
      }
    }

    return listener.future;
  }

  private void beatForCompletions() {
    // For thread safetyness make an immutable copy of the map.
    // any task added later will be part of the next check;
    ImmutableMap<String, TaskCompletionListener> listeners = null;
    synchronized (completionListeners) {
      listeners = ImmutableMap.copyOf(completionListeners);
    }

    for (Map.Entry<String, TaskCompletionListener> e : listeners.entrySet()) {
      beatForCompletion(e.getKey(), e.getValue());
    }
  }

  private void beatForCompletion(String handle, TaskCompletionListener listener) {
    if (!listener.beatingSem.tryAcquire()) {
      return;
    }

    try {
      FindTaskReq req = FindTaskReq.newBuilder()
          .setHandle(Long.parseLong(handle))
          .setIsForDoneOnly(true)
          .build();

      ListenableFuture<TaskProgress> progressF = TortugaGrpc.newFutureStub(chan)
          // we make this long running so the server can later optimize...
          .withDeadlineAfter(30L, TimeUnit.SECONDS)
          .findTaskByHandle(req);

      Futures.addCallback(progressF, new FutureCallback<TaskProgress>() {
        @Override
        public void onSuccess(TaskProgress progress) {
          onTaskProgressSuccess(progress, handle, listener);
        }

        @Override
        public void onFailure(Throwable t) {
          listener.beatingSem.release();
          LOG.error("completion beats are failing", t);
        }
      });


    } catch (RuntimeException ex) {
      LOG.error("Scheduling completion beats is failing", ex);
      listener.beatingSem.release();
    }
  }

  private void onTaskProgressSuccess(TaskProgress progress, String handle, TaskCompletionListener listener) {
    if (!progress.getDone()) {
      listener.beatingSem.release();
      return;
    }

    synchronized (completionListeners) {
      // until then anyone asking for the future has gotten our listener as a result,
      // so necessarily we are not erasing another listener here.
      completionListeners.remove(handle);
    }

    // If the beat for completion had made a copy that included this task before our removal,
    // then we'd be fine because it won't be able to acquire the last sem that we'll never release :)

    Status status = Status.fromCodeValue(progress.getStatus().getCode());
    listener.future.set(status);
  }

  private void heartbeatForAllWorkers() {
    LOG.debug("sending heartbeat for {} workers", startedWorkers.size());

    if (!heartbeatSem.tryAcquire()) {
      return;
    }

    try {
      ImmutableList<Tortuga> started = null;
      synchronized (startedWorkers) {
        started = ImmutableList.copyOf(startedWorkers);
      }

      if (started.isEmpty()) {
        // what's the point? :)
        heartbeatSem.release();
        return;
      }

      HeartbeatReq.Builder req = HeartbeatReq.newBuilder();

      for (Tortuga tortuga : started) {
        req.addWorkerBeats(tortuga.toWorkerBeat());
      }

      ListenableFuture<Empty> done = TortugaGrpc.newFutureStub(chan)
          .withDeadlineAfter(15L, TimeUnit.SECONDS)
          .heartbeat(req.build());
      Futures.addCallback(done, new FutureCallback<Empty>() {
        @Override
        public void onSuccess(Empty result) {
          heartbeatSem.release();
        }

        @Override
        public void onFailure(Throwable t) {
          LOG.error("heartbeats are failing: ", t);
          heartbeatSem.release();
        }
      });
    } catch (RuntimeException ex) {
      heartbeatSem.release();
      LOG.error("Couldn't schedule a hearbeat", ex);
      throw ex;
    }
  }
}
