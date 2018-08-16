package io.tortuga;

public class TaskResult {

  private final String handle;
  private final boolean created;

  TaskResult(String handle, boolean created) {
    this.handle = handle;
    this.created = created;
  }

  public String handle() {
    return handle;
  }

  public boolean isCreated() {
    return created;
  }
}
