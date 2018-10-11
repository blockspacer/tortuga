package io.tortuga;

import io.grpc.Metadata;
import io.grpc.stub.AbstractStub;
import io.grpc.stub.MetadataUtils;

/**
 * Utils for generated tortuga handlers.
 */
public class TortugaHandlers {
  private static final Metadata.Key<String> keyHandle = Metadata.Key.of("tortuga-handle", Metadata.ASCII_STRING_MARSHALLER);

  public static <T extends AbstractStub<T>> T withHandleMetadata(T stub, TortugaContext ctx) {
    Metadata metadata = new Metadata();
    metadata.put(keyHandle, ctx.handle());

    return MetadataUtils.attachHeaders(stub, metadata);
  }

  // do not instantiate.
  private TortugaHandlers() {
  }
}
