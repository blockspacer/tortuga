package io.tortuga;

import com.google.common.util.concurrent.ListenableFuture;
import com.google.protobuf.Any;

import io.grpc.Status;

public interface TaskHandler {
  ListenableFuture<Status> execute(Any data, TortugaContext ctx);
}
