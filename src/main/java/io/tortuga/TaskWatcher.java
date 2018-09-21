package io.tortuga;

import com.google.common.util.concurrent.FutureCallback;
import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;

import io.tortuga.TortugaProto.TaskProgress;

public class TaskWatcher {
  private final TortugaConnection conn;
  private TaskProgress progress;

  TaskWatcher(TaskProgress progress, TortugaConnection conn) {
    this.progress = progress;
    this.conn = conn;
  }

  public TaskProgress getLastProgress() {
    return progress;
  }

  public TaskProgress refreshProgress() {
    progress = conn.getProgress(progress.getHandle());
    return progress;
  }

  public ListenableFuture<TaskProgress> refreshProgressAsync() {
    ListenableFuture<TaskProgress> progressF = conn.getProgressAsync(progress.getHandle());
    Futures.addCallback(progressF, new FutureCallback<TaskProgress>() {
      @Override
      public void onSuccess(TaskProgress result) {
        progress = result;
      }

      @Override
      public void onFailure(Throwable t) {

      }
    });

    return progressF;
  }
}
