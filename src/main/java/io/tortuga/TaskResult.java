package io.tortuga;

public class TaskResult {

  private final String handle;
  private final boolean created;
  private final TortugaConnection conn;

  TaskResult(String handle, boolean created, TortugaConnection conn) {
    this.handle = handle;
    this.created = created;
    this.conn = conn;
  }

  public String handle() {
    return handle;
  }

  public boolean isCreated() {
    return created;
  }

  /**
   * @return whether the created task is now done.
   */
  public boolean isDone() {
    return conn.isDone(handle);
  }
}
