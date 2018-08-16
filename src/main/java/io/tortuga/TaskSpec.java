package io.tortuga;

import java.util.OptionalInt;

public class TaskSpec {
  final String id;
  OptionalInt maxRetries = OptionalInt.empty();
  OptionalInt priority = OptionalInt.empty();

  private TaskSpec(String id) {
    this.id = id;
  }

  public static TaskSpec ofId(String id) {
    return new TaskSpec(id);
  }

  public TaskSpec withMaxRetries(int val) {
    maxRetries = OptionalInt.of(val);
    return this;
  }

  public TaskSpec withPriority(int val) {
    priority = OptionalInt.of(val);
    return this;
  }
}
