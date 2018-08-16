package io.tortuga;

public class TortugaContext {
  private final StringBuilder sb = new StringBuilder();

  public void log(String s) {
    sb.append(s);
  }

  String logs() {
    return sb.toString();
  }
}
