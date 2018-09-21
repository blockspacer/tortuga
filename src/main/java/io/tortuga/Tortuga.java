package io.tortuga;

import com.google.common.util.concurrent.FutureCallback;
import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.common.util.concurrent.ListenableScheduledFuture;
import com.google.common.util.concurrent.ListeningExecutorService;
import com.google.common.util.concurrent.ListeningScheduledExecutorService;
import com.google.common.util.concurrent.MoreExecutors;
import com.google.common.util.concurrent.ThreadFactoryBuilder;
import com.google.protobuf.Empty;

import io.grpc.Channel;
import io.grpc.Status;
import io.tortuga.TortugaProto.CompleteTaskReq;
import io.tortuga.TortugaProto.TaskReq;
import io.tortuga.TortugaProto.TaskResp;
import io.tortuga.TortugaProto.Worker;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.UUID;
import java.util.concurrent.Executors;
import java.util.concurrent.Semaphore;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

public class Tortuga {
  private static final Logger LOG = LoggerFactory.getLogger(Tortuga.class);

  private boolean started;
  private final Channel chan;
  // Single thread that handles the heartbeat and the GRPC communication to server.
  private final ListeningScheduledExecutorService maintenanceService;
  // Executor where performing tasks.
  private ListeningExecutorService workersPool;

  private Worker worker;

  private int concurrency = 4;
  private Semaphore semaphore;

  private ListenableScheduledFuture<?> heartbeatSchedulation;
  private ListenableScheduledFuture<?> pollSchedulation;
  private final AtomicBoolean shuttingDown = new AtomicBoolean();

  private final TaskHandlerRegistry registry = new TaskHandlerRegistry();

  Tortuga(String workerId, Channel chan, ListeningScheduledExecutorService maintenanceService) {
    this.chan = chan;
    this.maintenanceService = maintenanceService;

    worker = Worker.newBuilder()
        .setWorkerId(workerId)
        .setUuid(UUID.randomUUID().toString())
        .build();
  }

  public Tortuga withConcurrency(int nThreads) {
    concurrency = nThreads;
    return this;
  }

  public void addService(Service s) {
    s.register(registry);
  }

  public void start() {
    if (started) {
      throw new IllegalStateException("This Tortuga was already started.");
    }

    started = true;

    worker = worker.toBuilder()
        .addAllCapabilities(registry.capabilities())
        .build();

    if (workersPool == null) {
      // If the caller didn't specify a executor service for workers we default to a cached thread pool.
      ThreadFactory workersTf = new ThreadFactoryBuilder()
          .setNameFormat("tortuga-worker-%d")
          .build();
      workersPool = MoreExecutors.listeningDecorator(Executors.newCachedThreadPool(workersTf));
    }

    semaphore = new Semaphore(concurrency);

    heartbeatSchedulation = maintenanceService.scheduleWithFixedDelay(() -> {
      heartbeat();
    }, 1L, 1L, TimeUnit.SECONDS);

    pollSchedulation = maintenanceService.scheduleWithFixedDelay(() -> {
      try {
        pollTask();
      } catch (Exception ex) {
        ex.printStackTrace();
      }
    }, 500L, 500L, TimeUnit.MILLISECONDS);
  }

  private void heartbeat() {
    LOG.debug("sending heartbeat");
    ListenableFuture<Empty> done = TortugaGrpc.newFutureStub(chan)
        .withDeadlineAfter(5L, TimeUnit.SECONDS)
        .heartbeat(worker);
    Futures.addCallback(done, new FutureCallback<Empty>() {
      @Override
      public void onSuccess(Empty result) {
      }

      @Override
      public void onFailure(Throwable t) {
        LOG.error("heartbeats are failing: ", t);
      }
    });
  }

  private void pollTask() {
    LOG.debug("polling for tortuga task");
    if (!semaphore.tryAcquire()) {
      LOG.debug("no more workers");
      return;
    }

    try {
      pollTaskHoldingSem();
    } catch (RuntimeException ex) {
      LOG.error("couldn't poll task", ex);
      semaphore.release();
    }
  }

  private void pollTaskHoldingSem() {
    TaskReq req = TaskReq.newBuilder()
        .setWorker(worker)
        .build();
    ListenableFuture<TaskResp> respF = TortugaGrpc.newFutureStub(chan)
        .withDeadlineAfter(5L, TimeUnit.SECONDS)
        .requestTask(req);

    Futures.addCallback(respF, new FutureCallback<TaskResp>() {
      @Override
      public void onSuccess(TaskResp resp) {
        if (resp.getNone()) {
          LOG.debug("no task available :)");
          semaphore.release();
          return;
        }

        LOG.debug("got task: {}", resp);
        TaskHandler handler = registry.get(resp.getType());
        if (handler == null) {
          // something is really wrong at the server level if we get tasks that we shouldn't get.
          LOG.error("Unreachable!", new AssertionError());
          semaphore.release();
          return;
        }

        ListenableFuture<Void> xf = Futures.immediateFuture(null);
        TortugaContext ctx = new TortugaContext(resp.getHandle(), resp.getRetryCtx(), chan, worker);
        ListenableFuture<Status> statusF = Futures.transformAsync(xf, x-> handler.execute(resp.getData(), ctx), workersPool);
        statusF = Futures.catching(statusF, Exception.class, ex-> {
          StringBuilder sb = new StringBuilder();
          sb.append("Uhoh, tortuga handler for type: ").append(resp.getType()).append(" failed.");
          sb.append('\n');
          sb.append("Task that failed:\n");
          sb.append("Handle: ").append(resp.getHandle()).append('\n');
          sb.append("Id: ").append(resp.getId()).append('\n');
          LOG.error(sb.toString(), ex);
          return Status.fromThrowable(ex);
        });

        ListenableFuture<Empty> doneAck = Futures.transformAsync(statusF, st -> {
          CompleteTaskReq.Builder req = CompleteTaskReq.newBuilder();
          req.setHandle(resp.getHandle());
          req.setLogs(ctx.logs());
          req.setWorker(worker);
          req.setCode(st.getCode().value());
          req.setOutput(ctx.output());

          StringWriter output = new StringWriter();
          PrintWriter p = new PrintWriter(output);
          if (st.getDescription() != null) {
            output.append(st.getDescription()).append('\n');
          }

          if (st.getCause() != null) {
            st.getCause().printStackTrace(p);
          }

          req.setErrorMessage(output.toString());

          return TortugaGrpc.newFutureStub(chan)
              .withDeadlineAfter(30L, TimeUnit.SECONDS)
              .completeTask(req.build());
        });

        Futures.addCallback(doneAck, new FutureCallback<Empty>() {
          @Override
          public void onSuccess(Empty result) {
            semaphore.release();
            LOG.debug("completed task {}.", resp.getHandle());
          }

          @Override
          public void onFailure(Throwable t) {
            semaphore.release();
            LOG.error("couldn't complete task {}.", resp.getHandle(), t);
          }
        });
      }

      @Override
      public void onFailure(Throwable t) {
        semaphore.release();
        LOG.error("error while requesting tortuga task", t);
      }
    });
  }

  public void shutdown() {
    shuttingDown.set(true);
    heartbeatSchedulation.cancel(false);
    pollSchedulation.cancel(false);

    try {
      semaphore.acquire(concurrency);
      workersPool.shutdown();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
      throw new IllegalStateException("interrupted while shuting down", ex);
    }
  }
}
