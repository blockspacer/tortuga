package io.tortuga;

import com.google.protobuf.Duration;

import java.util.Optional;
import java.util.OptionalInt;

public class TaskSpec {
  final String id;
  OptionalInt maxRetries = OptionalInt.empty();
  OptionalInt priority = OptionalInt.empty();
  Optional<Duration> duration = Optional.empty();

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

  public TaskSpec withDelayInSeconds(int seconds) {
    duration = Optional.of(Duration.newBuilder()
        .setSeconds(seconds)
        .build());
    return this;
  }
}
