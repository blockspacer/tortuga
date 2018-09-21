package io.tortuga;

import com.google.common.collect.ImmutableList;
import com.google.common.util.concurrent.FutureCallback;
import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.common.util.concurrent.SettableFuture;
import com.google.common.util.concurrent.Uninterruptibles;
import com.google.protobuf.Empty;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.Status;
import io.grpc.StatusRuntimeException;
import io.tortuga.TortugaProto.TaskProgress;
import io.tortuga.test.TestService2Tortuga;
import io.tortuga.test.TestServiceTortuga;
import io.tortuga.test.TestServiceTortuga.ImplBase;
import io.tortuga.test.TortugaProto.TestMessage;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.annotation.Nullable;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class TortugaIntegrationTest {
  private static final Logger LOG = LoggerFactory.getLogger(TortugaIntegrationTest.class);

  @Before
  public void startTortuga() throws IOException {
    startTortuga(false);
  }

  void startTortuga(boolean keepDb) throws IOException {
    System.out.println("starting tortuga...");
    System.out.println("we are in dir: " + Paths.get("").toAbsolutePath());

    Path testDir = Paths.get("test_dir");
    if (!Files.exists(testDir)) {
      Files.createDirectory(testDir);
    }

    Path tortugaDB = testDir.resolve("tortuga.db");
    Path tortugaLogs = testDir.resolve("tortuga.logs");

    if (Files.exists(tortugaDB) && !keepDb) {
      Files.delete(tortugaDB);
    }

    if (Files.exists(tortugaLogs)) {
      Files.delete(tortugaLogs);
    }

    List<String> tortugaCmd = new ArrayList<>();
    tortugaCmd.add("./tortuga_main");
    tortugaCmd.add("--logtostderr");
    tortugaCmd.add("--v");
    tortugaCmd.add("5");
    tortugaCmd.add("--addr");
    tortugaCmd.add("127.0.0.1");
    tortugaCmd.add("--db_file");
    tortugaCmd.add("test_dir/tortuga.db");
    tortugaCmd.add("--firestore_collection");
    tortugaCmd.add("tortuga_tests");

    new ProcessBuilder()
        .command(tortugaCmd)
        .redirectError(tortugaLogs.toFile())
        .start();

    System.out.println("Tortuga logs are written to: " + tortugaLogs.toAbsolutePath().toString());

    // Wait until it is up...
    ManagedChannel chan = ManagedChannelBuilder.forAddress("127.0.0.1", 4000)
        .usePlaintext()
        .build();
    while (true) {
      try {
        TortugaGrpc.newBlockingStub(chan)
            .withDeadlineAfter(100L, TimeUnit.MILLISECONDS)
            .ping(Empty.getDefaultInstance());
        break;
      } catch (StatusRuntimeException ex) {
        Uninterruptibles.sleepUninterruptibly(5L, TimeUnit.MILLISECONDS);
      }
    }

    System.out.println("Tortuga server is up");
  }

  @After
  public void stopTortuga() {
    ManagedChannel chan = ManagedChannelBuilder.forAddress("127.0.0.1", 4000)
        .usePlaintext()
        .build();
    TortugaGrpc.newBlockingStub(chan)
        .withDeadlineAfter(5L, TimeUnit.SECONDS)
        .quitQuitQuit(Empty.getDefaultInstance());
  }

  // The most basic test of publishing and receiving messages.
  @Test
  public void testPubSub() {
    CountDownLatch latch = new CountDownLatch(100);
    TortugaConnection conn = TortugaConnection.newConnection("127.0.0.1", 4000);
    Tortuga tortuga = conn.newWorker("test_worker");
    tortuga.withConcurrency(4);
    Set<String> found = Collections.synchronizedSet(new HashSet<>());

    TestServiceTortuga.ImplBase handler = new TestServiceTortuga.ImplBase() {
      @Override
      public ListenableFuture<Status> handleCustomMessage(TestMessage t, TortugaContext ctx) {
        LOG.info("received task to handle: {}", t);
        found.add(t.getId());
        latch.countDown();
        return Futures.immediateFuture(Status.OK);
      }
    };

    tortuga.addService(handler);
    tortuga.start();

    Set<String> expected = new HashSet<>();
    TestServiceTortuga.Publisher publisher = TestServiceTortuga.newPublisher(conn);
    TaskResult task = null;
    for (int i = 1; i <= 100; ++i) {
      TestMessage testMessage = TestMessage.newBuilder()
          .setId("field_" + i)
          .build();
      expected.add("field_" + i);

      // we grab number 50 task because it is certain that it shall be done once latch is waited on.
      if (i == 50) {
        task = publisher.publishHandleCustomMessageTask(TaskSpec.ofId("TestTask" + i), testMessage);
      } else {
        publisher.publishHandleCustomMessageTask(TaskSpec.ofId("TestTask" + i), testMessage);
      }
    }

    Uninterruptibles.awaitUninterruptibly(latch);
    List<String> expectedList = new ArrayList<>(expected);
    Collections.sort(expectedList);

    List<String> foundList = new ArrayList<>(found);
    Collections.sort(foundList);

    Assert.assertEquals(expectedList, foundList);
    Assert.assertTrue(task.isDone());

    tortuga.shutdown();
    conn.shutdown();
  }

  // Publish messages to the same id, ensures only one is executed.
  @Test
  public void testDedup() {
    CountDownLatch latch = new CountDownLatch(2);
    TortugaConnection conn = TortugaConnection.newConnection("127.0.0.1", 4000);
    Tortuga tortuga = conn.newWorker("test_worker");
    tortuga.withConcurrency(4);
    Set<String> found = Collections.synchronizedSet(new HashSet<>());

    TestServiceTortuga.ImplBase handler = new TestServiceTortuga.ImplBase() {
      @Override
      public ListenableFuture<Status> handleCustomMessage(TestMessage t, TortugaContext ctx) {
        LOG.info("received task to handle: {}", t);
        found.add(t.getId());
        latch.countDown();
        return Futures.immediateFuture(Status.OK);
      }
    };

    tortuga.addService(handler);

    TestServiceTortuga.Publisher publisher = TestServiceTortuga.newPublisher(conn);
    for (int i = 1; i <= 10; ++i) {
      TestMessage testMessage = TestMessage.newBuilder()
          .setId("field_" + 1)
          .build();
      publisher.publishHandleCustomMessageTask(TaskSpec.ofId("TestTask_one"), testMessage);
    }

    tortuga.start();

    // If messages are deduped, the latch won't come to 2 because they all have the same id.
    // If messages are not correctly deduped it would have.
    Assert.assertFalse(Uninterruptibles.awaitUninterruptibly(latch, 3L, TimeUnit.SECONDS));

    TestMessage testMessage = TestMessage.newBuilder()
        .setId("field_" + 2)
        .build();
    publisher.publishHandleCustomMessageTask(TaskSpec.ofId("TestTask_two"), testMessage);
    Uninterruptibles.awaitUninterruptibly(latch);

    List<String> expectedList = ImmutableList.of("field_1", "field_2");

    List<String> foundList = new ArrayList<>(found);
    Collections.sort(foundList);

    Assert.assertEquals(expectedList, foundList);

    tortuga.shutdown();
    conn.shutdown();
  }

  // we checkout some tasks and then that worker "dies" and we check that the new one is getting the tasks.
  @Test
  public void testWorkerDeath() {
    TortugaConnection conn = TortugaConnection.newConnection("127.0.0.1", 4000);
    Tortuga tortuga = conn.newWorker("test_worker");
    tortuga.withConcurrency(4);
    CountDownLatch checkedAllFour = new CountDownLatch(4);
    tortuga.addService(new ImplBase() {
      @Override
      public ListenableFuture<Status> handleCustomMessage(TestMessage t, TortugaContext ctx) {
        LOG.info("received task to handle: {}", t);
        checkedAllFour.countDown();
        SettableFuture<Status> f = SettableFuture.create();
        return f;
      }
    });

    tortuga.start();

    Set<String> expected = new HashSet<>();
    TestServiceTortuga.Publisher publisher = TestServiceTortuga.newPublisher(conn);
    for (int i = 1; i <= 10; ++i) {
      TestMessage testMessage = TestMessage.newBuilder()
          .setId("field_" + i)
          .build();
      expected.add("field_" + i);
      publisher.publishHandleCustomMessageTask(TaskSpec.ofId("TestTask" + i), testMessage);
    }

    Uninterruptibles.awaitUninterruptibly(checkedAllFour);
    LOG.info("Tortuga 1 received 4 messages...");

    // This will never return so we do it in a thread.
    new Thread(() -> {
      tortuga.shutdown();
    }).start();

    TortugaConnection conn2 = TortugaConnection.newConnection("127.0.0.1", 4000);
    Tortuga tortuga2 = conn2.newWorker("test_worker");
    tortuga2.withConcurrency(4);
    CountDownLatch latch = new CountDownLatch(10);
    Set<String> found = Collections.synchronizedSet(new HashSet<>());
    tortuga2.addService(new ImplBase() {
      @Override
      public ListenableFuture<Status> handleCustomMessage(TestMessage t, TortugaContext ctx) {
        LOG.info("received task to handle: {}", t);
        found.add(t.getId());
        latch.countDown();
        return Futures.immediateFuture(Status.OK);
      }
    });

    tortuga2.start();
    Uninterruptibles.awaitUninterruptibly(latch);

    List<String> expectedList = new ArrayList<>(expected);
    Collections.sort(expectedList);

    List<String> foundList = new ArrayList<>(found);
    Collections.sort(foundList);

    Assert.assertEquals(expectedList, foundList);
    tortuga2.shutdown();
  }

  @Test
  public void testRetries() {
    TortugaConnection conn = TortugaConnection.newConnection("127.0.0.1", 4000);
    Tortuga tortuga = conn.newWorker("test_worker");
    tortuga.withConcurrency(4);

    CountDownLatch sawFirstSevenTimes = new CountDownLatch(7);
    CountDownLatch sawSecondThreeTimes = new CountDownLatch(3);

    AtomicInteger sawFirst = new AtomicInteger();
    AtomicInteger sawSecond = new AtomicInteger();

    tortuga.addService(new ImplBase() {
      @Override
      public ListenableFuture<Status> handleCustomMessage(TestMessage t, TortugaContext ctx) {
        LOG.info("received task to handle: {}", t);
        if ("field_1".equals(t.getId())) {
          int saw = sawFirst.incrementAndGet();
          sawFirstSevenTimes.countDown();
          if (saw < 7) {
            return Futures.immediateFuture(Status.INTERNAL);
          } else {
            return Futures.immediateFuture(Status.OK);
          }
        } else {
          sawSecond.incrementAndGet();
          sawSecondThreeTimes.countDown();
          return Futures.immediateFuture(Status.INTERNAL);
        }
      }
    });

    tortuga.start();

    TestServiceTortuga.Publisher publisher = TestServiceTortuga.newPublisher(conn);
    for (int i = 1; i <= 2; ++i) {
      TestMessage testMessage = TestMessage.newBuilder()
          .setId("field_" + i)
          .build();
      if (i == 1) {
        publisher.publishHandleCustomMessageTask(TaskSpec.ofId("TestTask" + i).withMaxRetries(10), testMessage);
      } else {
        publisher.publishHandleCustomMessageTask(TaskSpec.ofId("TestTask" + i).withMaxRetries(3), testMessage);
      }
    }

    Uninterruptibles.awaitUninterruptibly(sawFirstSevenTimes);
    Uninterruptibles.awaitUninterruptibly(sawSecondThreeTimes);
    // we will sleep a bit here to ensure that we do not receive more messages than planned.
    Uninterruptibles.sleepUninterruptibly(2L, TimeUnit.SECONDS);

    Assert.assertEquals(7, sawFirst.get());
    Assert.assertEquals(3, sawSecond.get());
    tortuga.shutdown();
    conn.shutdown();
  }

  @Test
  public void testCompletionListening() {
    TortugaConnection conn = TortugaConnection.newConnection("127.0.0.1", 4000);

    TestServiceTortuga.Publisher publisher = TestServiceTortuga.newPublisher(conn);
    CountDownLatch latch = new CountDownLatch(100);

    for (int i = 0; i < 100; ++i) {
      publisher.publishHandleCustomMessageTask(TaskSpec.ofId("SomeTask" + i), TestMessage.getDefaultInstance());
      TaskResult handle = publisher.publishHandleCustomMessageTask(TaskSpec.ofId("SomeId"), TestMessage.getDefaultInstance());
      ListenableFuture<Status> completionF = handle.completionFuture();
      
      Futures.addCallback(completionF, new FutureCallback<Status>() {
        @Override
        public void onSuccess(@Nullable Status result) {
          latch.countDown();
        }

        @Override
        public void onFailure(Throwable t) {
        }
      });
    }

    Assert.assertFalse(Uninterruptibles.awaitUninterruptibly(latch, 5L, TimeUnit.SECONDS));

    Tortuga tortuga = conn.newWorker("test_worker");
    tortuga.withConcurrency(4);
    tortuga.addService(new ImplBase() {
      @Override
      public ListenableFuture<Status> handleCustomMessage(TestMessage t, TortugaContext ctx) {
        return Futures.immediateFuture(Status.OK);
      }
    });
    tortuga.start();

    Assert.assertTrue(Uninterruptibles.awaitUninterruptibly(latch, 60L, TimeUnit.SECONDS));
    tortuga.shutdown();
    conn.shutdown();
  }

  @Test
  public void testCompletionWithServerDeath() throws Exception {
    TortugaConnection conn = TortugaConnection.newConnection("127.0.0.1", 4000);

    TestServiceTortuga.Publisher publisher = TestServiceTortuga.newPublisher(conn);
    CountDownLatch latch = new CountDownLatch(100);

    for (int i = 0; i < 100; ++i) {
      publisher.publishHandleCustomMessageTask(TaskSpec.ofId("SomeTask" + i), TestMessage.getDefaultInstance());
      TaskResult handle = publisher.publishHandleCustomMessageTask(TaskSpec.ofId("SomeId"), TestMessage.getDefaultInstance());
      ListenableFuture<Status> completionF = handle.completionFuture();

      Futures.addCallback(completionF, new FutureCallback<Status>() {
        @Override
        public void onSuccess(@Nullable Status result) {
          latch.countDown();
        }

        @Override
        public void onFailure(Throwable t) {
        }
      });
    }

    Assert.assertFalse(Uninterruptibles.awaitUninterruptibly(latch, 5L, TimeUnit.SECONDS));
    stopTortuga();
    Uninterruptibles.sleepUninterruptibly(1L, TimeUnit.SECONDS);
    startTortuga(true);

    Tortuga tortuga = conn.newWorker("test_worker");
    tortuga.withConcurrency(4);
    tortuga.addService(new ImplBase() {
      @Override
      public ListenableFuture<Status> handleCustomMessage(TestMessage t, TortugaContext ctx) {
        return Futures.immediateFuture(Status.OK);
      }
    });
    tortuga.start();

    Assert.assertTrue(Uninterruptibles.awaitUninterruptibly(latch, 60L, TimeUnit.SECONDS));
    tortuga.shutdown();
    conn.shutdown();
  }

  /**
   * Tests that we only get the tasks that we asked for.
   */
  @Test
  public void testRequestedType() {
    TortugaConnection conn = TortugaConnection.newConnection("127.0.0.1", 4000);

    TestService2Tortuga.Publisher publisher = TestService2Tortuga.newPublisher(conn);
    CountDownLatch latch = new CountDownLatch(10);

    for (int i = 0; i < 10; ++i) {
      publisher.publishHandleCustomMessage2Task(TaskSpec.ofId("SomeTask" + i), TestMessage.getDefaultInstance());
    }

    Tortuga tortuga = conn.newWorker("test_worker");
    tortuga.withConcurrency(4);
    tortuga.addService(new ImplBase() {
      @Override
      public ListenableFuture<Status> handleCustomMessage(TestMessage t, TortugaContext ctx) {
        latch.countDown();
        return Futures.immediateFuture(Status.OK);
      }
    });
    tortuga.start();

    Assert.assertFalse(Uninterruptibles.awaitUninterruptibly(latch, 5L, TimeUnit.SECONDS));

    Tortuga tortuga2 = conn.newWorker("test_worker2");
    tortuga2.addService(new TestService2Tortuga.ImplBase() {
      @Override
      public ListenableFuture<Status> handleCustomMessage2(TestMessage t, TortugaContext ctx) {
        latch.countDown();
        return Futures.immediateFuture(Status.OK);
      }
    });
    tortuga2.start();

    Assert.assertTrue(Uninterruptibles.awaitUninterruptibly(latch, 50L, TimeUnit.SECONDS));
    tortuga.shutdown();
    tortuga2.shutdown();
    conn.shutdown();
  }

  @Test
  public void testDelayedTasks() {
    TortugaConnection conn = TortugaConnection.newConnection("127.0.0.1", 4000);
    CountDownLatch latch = new CountDownLatch(10);

    Tortuga tortuga = conn.newWorker("test_worker");
    tortuga.withConcurrency(4);
    tortuga.addService(new ImplBase() {
      @Override
      public ListenableFuture<Status> handleCustomMessage(TestMessage t, TortugaContext ctx) {
        latch.countDown();
        return Futures.immediateFuture(Status.OK);
      }
    });
    tortuga.start();

    TestServiceTortuga.Publisher publisher = TestServiceTortuga.newPublisher(conn);

    for (int i = 0; i < 10; ++i) {
      publisher.publishHandleCustomMessageTask(TaskSpec.ofId("SomeTask" + i).withDelayInSeconds(10), TestMessage.getDefaultInstance());
    }

    // Because of the delay...
    Assert.assertFalse(Uninterruptibles.awaitUninterruptibly(latch, 9L, TimeUnit.SECONDS));
    Assert.assertTrue(Uninterruptibles.awaitUninterruptibly(latch, 60L, TimeUnit.SECONDS));
    tortuga.shutdown();
    conn.shutdown();
  }

  @Test
  public void testUpdateProgress() {
    TortugaConnection conn = TortugaConnection.newConnection("127.0.0.1", 4000);
    CountDownLatch latch = new CountDownLatch(10);
    Lock lock = new ReentrantLock();
    lock.lock();

    Tortuga tortuga = conn.newWorker("test_worker");
    tortuga.withConcurrency(4);
    tortuga.addService(new ImplBase() {
      @Override
      public ListenableFuture<Status> handleCustomMessage(TestMessage t, TortugaContext ctx) {
        ctx.updateProgress(50.0f, "The task is done at 50%");

        lock.lock();
        latch.countDown();
        lock.unlock();
        ctx.setOutput("The output for task: " + t.getId());
        return Futures.immediateFuture(Status.OK);
      }
    });

    tortuga.start();

    TestServiceTortuga.Publisher publisher = TestServiceTortuga.newPublisher(conn);

    for (int i = 0; i < 10; ++i) {
      publisher.publishHandleCustomMessageTask(TaskSpec.ofId("SomeTask" + i).withModule("firestore"), TestMessage.newBuilder()
           .setId(Integer.toString(i))
           .build());
    }

    // Because we are locked.
    Assert.assertFalse(Uninterruptibles.awaitUninterruptibly(latch, 3L, TimeUnit.SECONDS));

    TestServiceTortuga.TaskManager manager = TestServiceTortuga.newTaskManager(conn);
    Assert.assertFalse(manager.findHandleTaskTask("SomeTask0").isPresent());

    Optional<TaskWatcher> watcherOpt = manager.findHandleCustomMessageTask("SomeTask0");
    Assert.assertTrue(watcherOpt.isPresent());
    TaskWatcher watcher = watcherOpt.get();
    TaskProgress progress = watcher.refreshProgress();

    Assert.assertEquals(50.0F, progress.getProgress(), 0.001F);
    Assert.assertEquals("The task is done at 50%",  progress.getProgressMessage());

    lock.unlock();
    // They shall all proceed.
    Assert.assertTrue(Uninterruptibles.awaitUninterruptibly(latch, 60L, TimeUnit.SECONDS));

    progress = watcher.refreshProgress();
    Assert.assertEquals(100.0F, progress.getProgress(), 0.001F);

    tortuga.shutdown();
    conn.shutdown();
  }
}
