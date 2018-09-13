package io.tortuga;

import io.tortuga.TortugaProto.TaskProgress;

public class TaskWatcher {
  private final String handle;
  private final TortugaConnection conn;

  TaskWatcher(String handle, TortugaConnection conn) {
    this.handle = handle;
    this.conn = conn;
  }

  public TaskProgress getProgress() {
    return conn.getProgress(handle);
  }
}
