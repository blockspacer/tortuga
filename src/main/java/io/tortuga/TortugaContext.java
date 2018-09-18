package io.tortuga;

import com.google.common.base.Strings;
import com.google.protobuf.FloatValue;
import com.google.protobuf.StringValue;

import io.grpc.Channel;
import io.tortuga.TortugaProto.UpdateProgressReq;
import io.tortuga.TortugaProto.Worker;

import java.util.concurrent.TimeUnit;

public class TortugaContext {
  private final StringBuilder sb = new StringBuilder();

  private final String handle;
  private final Channel tortugaChan;
  private final Worker worker;

  private String output;

  TortugaContext(String handle,
                 Channel chan,
                 Worker worker) {
    this.handle = handle;
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

    updateProgress(req);
  }

  public void updateProgress(float progress, String progressMsg) {
    UpdateProgressReq.Builder req = UpdateProgressReq.newBuilder()
        .setProgress(FloatValue.of(progress))
        .setProgressMessage(StringValue.of(progressMsg));

    updateProgress(req);
  }

  public void updateProgressMsg(String progressMsg) {
    UpdateProgressReq.Builder req = UpdateProgressReq.newBuilder()
        .setProgressMessage(StringValue.of(progressMsg));

    updateProgress(req);
  }

  private void updateProgress(UpdateProgressReq.Builder req) {
    req.setHandle(handle);
    req.setWorker(worker);

    TortugaGrpc.newBlockingStub(tortugaChan)
        .withDeadlineAfter(30L, TimeUnit.SECONDS)
        .updateProgress(req.build());
  }
}
