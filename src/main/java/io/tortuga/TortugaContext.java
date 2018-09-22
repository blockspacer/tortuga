package io.tortuga;

import com.google.common.base.Functions;
import com.google.common.base.Strings;
import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;
import com.google.protobuf.Empty;
import com.google.protobuf.FloatValue;
import com.google.protobuf.StringValue;

import io.grpc.Channel;
import io.tortuga.TortugaProto.TaskResp.RetryContext;
import io.tortuga.TortugaProto.UpdateProgressReq;
import io.tortuga.TortugaProto.Worker;

import java.util.concurrent.TimeUnit;

public class TortugaContext {
  private final StringBuilder sb = new StringBuilder();

  private final String handle;
  private final RetryContext retryCtx;
  private final Channel tortugaChan;
  private final Worker worker;

  private String output;

  TortugaContext(String handle,
                 RetryContext retryCtx,
                 Channel chan,
                 Worker worker) {
    this.handle = handle;
    this.retryCtx = retryCtx;
    this.tortugaChan = chan;
    this.worker = worker;
  }

  public void log(String s) {
    sb.append(s);
  }

  String logs() {
    return sb.toString();
  }

  public void setOutput(String output) {
    this.output = output;
  }

  String output() {
    return Strings.nullToEmpty(output);
  }

  public void updateProgress(float progress) {
    UpdateProgressReq.Builder req = UpdateProgressReq.newBuilder()
        .setProgress(FloatValue.of(progress));

    doUpdateProgress(req);
  }

  public void updateProgress(float progress, String progressMsg) {
    UpdateProgressReq.Builder req = UpdateProgressReq.newBuilder()
        .setProgress(FloatValue.of(progress))
        .setProgressMessage(StringValue.of(progressMsg));

    doUpdateProgress(req);
  }

  public void updateProgress(UpdateProgressReq.Builder req) {
    doUpdateProgress(req);
  }

  public ListenableFuture<Void> updateProgressAsync(UpdateProgressReq.Builder req) {
    req.setHandle(handle);
    req.setWorker(worker);

    ListenableFuture<Empty> doneF = TortugaGrpc.newFutureStub(tortugaChan)
        .withDeadlineAfter(30L, TimeUnit.SECONDS)
        .updateProgress(req.build());
    return Futures.transform(doneF, Functions.constant(null));
  }

  public void updateProgressMsg(String progressMsg) {
    UpdateProgressReq.Builder req = UpdateProgressReq.newBuilder()
        .setProgressMessage(StringValue.of(progressMsg));

    updateProgress(req);
  }

  private void doUpdateProgress(UpdateProgressReq.Builder req) {
    req.setHandle(handle);
    req.setWorker(worker);

    TortugaGrpc.newBlockingStub(tortugaChan)
        .withDeadlineAfter(30L, TimeUnit.SECONDS)
        .updateProgress(req.build());
  }

  public RetryContext retryCtx() {
    return retryCtx;
  }
}
