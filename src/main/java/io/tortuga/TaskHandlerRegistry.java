package io.tortuga;

import java.util.HashMap;
import java.util.Map;

public class TaskHandlerRegistry {
  private final Map<String, TaskHandler> handlers = new HashMap<>();

  public void registerHandler(String type, TaskHandler handler) {
    handlers.put(type, handler);
  }

  TaskHandler get(String type) {
    return handlers.get(type);
  }

  Iterable<String> capabilities() {
    return handlers.keySet();
  }
}
