package io.tortuga;

import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.common.util.concurrent.SettableFuture;
import com.google.common.util.concurrent.Uninterruptibles;
import com.google.protobuf.Empty;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.Status;
import io.grpc.StatusRuntimeException;
import io.tortuga.test.TestServiceTortuga;
import io.tortuga.test.TestServiceTortuga.ImplBase;
import io.tortuga.test.TortugaProto.TestMessage;

import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

public class TortugaIntegrationTest {
  private static final Logger LOG = LoggerFactory.getLogger(TortugaIntegrationTest.class);

  @Before
  public void startTortuga() throws IOException {
    System.out.println("starting tortuga...");
    System.out.println("we are in dir: " + Paths.get("").toAbsolutePath());

    Path testDir = Paths.get("test_dir");
    if (!Files.exists(testDir)) {
      Files.createDirectory(testDir);
    }

    Path tortugaDB = testDir.resolve("tortuga.db");
    Path tortugaLogs = testDir.resolve("tortuga.logs");

    if (Files.exists(tortugaDB)) {
      Files.delete(tortugaDB);
    }

    if (Files.exists(tortugaLogs)) {
      Files.delete(tortugaLogs);
    }

    List<String> tortugaCmd = new ArrayList<>();
    tortugaCmd.add("./tortuga_main");
    tortugaCmd.add("--logtostderr");
    tortugaCmd.add("--v");
    tortugaCmd.add("2");
    tortugaCmd.add("--addr");
    tortugaCmd.add("127.0.0.1");
    tortugaCmd.add("--db_file");
    tortugaCmd.add("test_dir/tortuga.db");

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
  public void stopTortuga() throws InterruptedException {
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
      public ListenableFuture<Status> HandleCustomMessage(TestMessage t, TortugaContext ctx) {
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
    for (int i = 1; i <= 100; ++i) {
      TestMessage testMessage = TestMessage.newBuilder()
          .setId("field_" + i)
          .build();
      expected.add("field_" + i);
      publisher.publishHandleCustomMessageTask(TaskSpec.ofId("TestTask" + i), testMessage);
    }

    Uninterruptibles.awaitUninterruptibly(latch);
    List<String> expectedList = new ArrayList<>(expected);
    Collections.sort(expectedList);

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
      public ListenableFuture<Status> HandleCustomMessage(TestMessage t, TortugaContext ctx) {
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
      public ListenableFuture<Status> HandleCustomMessage(TestMessage t, TortugaContext ctx) {
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
      public ListenableFuture<Status> HandleCustomMessage(TestMessage t, TortugaContext ctx) {
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
}
