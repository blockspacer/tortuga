// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: tortuga/tortuga_params.proto

package io.tortuga;

public final class TortugaParamsProto {
  private TortugaParamsProto() {}
  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistryLite registry) {
    registry.add(io.tortuga.TortugaParamsProto.bridge);
  }

  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistry registry) {
    registerAllExtensions(
        (com.google.protobuf.ExtensionRegistryLite) registry);
  }
  public interface TortugaBridgeOptsOrBuilder extends
      // @@protoc_insertion_point(interface_extends:tortuga.TortugaBridgeOpts)
      com.google.protobuf.MessageOrBuilder {

    /**
     * <code>optional int32 timeout_seconds = 1;</code>
     */
    boolean hasTimeoutSeconds();
    /**
     * <code>optional int32 timeout_seconds = 1;</code>
     */
    int getTimeoutSeconds();
  }
  /**
   * Protobuf type {@code tortuga.TortugaBridgeOpts}
   */
  public  static final class TortugaBridgeOpts extends
      com.google.protobuf.GeneratedMessageV3 implements
      // @@protoc_insertion_point(message_implements:tortuga.TortugaBridgeOpts)
      TortugaBridgeOptsOrBuilder {
  private static final long serialVersionUID = 0L;
    // Use TortugaBridgeOpts.newBuilder() to construct.
    private TortugaBridgeOpts(com.google.protobuf.GeneratedMessageV3.Builder<?> builder) {
      super(builder);
    }
    private TortugaBridgeOpts() {
      timeoutSeconds_ = 0;
    }

    @java.lang.Override
    public final com.google.protobuf.UnknownFieldSet
    getUnknownFields() {
      return this.unknownFields;
    }
    private TortugaBridgeOpts(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      this();
      if (extensionRegistry == null) {
        throw new java.lang.NullPointerException();
      }
      int mutable_bitField0_ = 0;
      com.google.protobuf.UnknownFieldSet.Builder unknownFields =
          com.google.protobuf.UnknownFieldSet.newBuilder();
      try {
        boolean done = false;
        while (!done) {
          int tag = input.readTag();
          switch (tag) {
            case 0:
              done = true;
              break;
            default: {
              if (!parseUnknownField(
                  input, unknownFields, extensionRegistry, tag)) {
                done = true;
              }
              break;
            }
            case 8: {
              bitField0_ |= 0x00000001;
              timeoutSeconds_ = input.readInt32();
              break;
            }
          }
        }
      } catch (com.google.protobuf.InvalidProtocolBufferException e) {
        throw e.setUnfinishedMessage(this);
      } catch (java.io.IOException e) {
        throw new com.google.protobuf.InvalidProtocolBufferException(
            e).setUnfinishedMessage(this);
      } finally {
        this.unknownFields = unknownFields.build();
        makeExtensionsImmutable();
      }
    }
    public static final com.google.protobuf.Descriptors.Descriptor
        getDescriptor() {
      return io.tortuga.TortugaParamsProto.internal_static_tortuga_TortugaBridgeOpts_descriptor;
    }

    protected com.google.protobuf.GeneratedMessageV3.FieldAccessorTable
        internalGetFieldAccessorTable() {
      return io.tortuga.TortugaParamsProto.internal_static_tortuga_TortugaBridgeOpts_fieldAccessorTable
          .ensureFieldAccessorsInitialized(
              io.tortuga.TortugaParamsProto.TortugaBridgeOpts.class, io.tortuga.TortugaParamsProto.TortugaBridgeOpts.Builder.class);
    }

    private int bitField0_;
    public static final int TIMEOUT_SECONDS_FIELD_NUMBER = 1;
    private int timeoutSeconds_;
    /**
     * <code>optional int32 timeout_seconds = 1;</code>
     */
    public boolean hasTimeoutSeconds() {
      return ((bitField0_ & 0x00000001) == 0x00000001);
    }
    /**
     * <code>optional int32 timeout_seconds = 1;</code>
     */
    public int getTimeoutSeconds() {
      return timeoutSeconds_;
    }

    private byte memoizedIsInitialized = -1;
    public final boolean isInitialized() {
      byte isInitialized = memoizedIsInitialized;
      if (isInitialized == 1) return true;
      if (isInitialized == 0) return false;

      memoizedIsInitialized = 1;
      return true;
    }

    public void writeTo(com.google.protobuf.CodedOutputStream output)
                        throws java.io.IOException {
      if (((bitField0_ & 0x00000001) == 0x00000001)) {
        output.writeInt32(1, timeoutSeconds_);
      }
      unknownFields.writeTo(output);
    }

    public int getSerializedSize() {
      int size = memoizedSize;
      if (size != -1) return size;

      size = 0;
      if (((bitField0_ & 0x00000001) == 0x00000001)) {
        size += com.google.protobuf.CodedOutputStream
          .computeInt32Size(1, timeoutSeconds_);
      }
      size += unknownFields.getSerializedSize();
      memoizedSize = size;
      return size;
    }

    @java.lang.Override
    public boolean equals(final java.lang.Object obj) {
      if (obj == this) {
       return true;
      }
      if (!(obj instanceof io.tortuga.TortugaParamsProto.TortugaBridgeOpts)) {
        return super.equals(obj);
      }
      io.tortuga.TortugaParamsProto.TortugaBridgeOpts other = (io.tortuga.TortugaParamsProto.TortugaBridgeOpts) obj;

      boolean result = true;
      result = result && (hasTimeoutSeconds() == other.hasTimeoutSeconds());
      if (hasTimeoutSeconds()) {
        result = result && (getTimeoutSeconds()
            == other.getTimeoutSeconds());
      }
      result = result && unknownFields.equals(other.unknownFields);
      return result;
    }

    @java.lang.Override
    public int hashCode() {
      if (memoizedHashCode != 0) {
        return memoizedHashCode;
      }
      int hash = 41;
      hash = (19 * hash) + getDescriptor().hashCode();
      if (hasTimeoutSeconds()) {
        hash = (37 * hash) + TIMEOUT_SECONDS_FIELD_NUMBER;
        hash = (53 * hash) + getTimeoutSeconds();
      }
      hash = (29 * hash) + unknownFields.hashCode();
      memoizedHashCode = hash;
      return hash;
    }

    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts parseFrom(
        java.nio.ByteBuffer data)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return PARSER.parseFrom(data);
    }
    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts parseFrom(
        java.nio.ByteBuffer data,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return PARSER.parseFrom(data, extensionRegistry);
    }
    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts parseFrom(
        com.google.protobuf.ByteString data)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return PARSER.parseFrom(data);
    }
    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts parseFrom(
        com.google.protobuf.ByteString data,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return PARSER.parseFrom(data, extensionRegistry);
    }
    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts parseFrom(byte[] data)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return PARSER.parseFrom(data);
    }
    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts parseFrom(
        byte[] data,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return PARSER.parseFrom(data, extensionRegistry);
    }
    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts parseFrom(java.io.InputStream input)
        throws java.io.IOException {
      return com.google.protobuf.GeneratedMessageV3
          .parseWithIOException(PARSER, input);
    }
    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts parseFrom(
        java.io.InputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws java.io.IOException {
      return com.google.protobuf.GeneratedMessageV3
          .parseWithIOException(PARSER, input, extensionRegistry);
    }
    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts parseDelimitedFrom(java.io.InputStream input)
        throws java.io.IOException {
      return com.google.protobuf.GeneratedMessageV3
          .parseDelimitedWithIOException(PARSER, input);
    }
    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts parseDelimitedFrom(
        java.io.InputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws java.io.IOException {
      return com.google.protobuf.GeneratedMessageV3
          .parseDelimitedWithIOException(PARSER, input, extensionRegistry);
    }
    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts parseFrom(
        com.google.protobuf.CodedInputStream input)
        throws java.io.IOException {
      return com.google.protobuf.GeneratedMessageV3
          .parseWithIOException(PARSER, input);
    }
    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts parseFrom(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws java.io.IOException {
      return com.google.protobuf.GeneratedMessageV3
          .parseWithIOException(PARSER, input, extensionRegistry);
    }

    public Builder newBuilderForType() { return newBuilder(); }
    public static Builder newBuilder() {
      return DEFAULT_INSTANCE.toBuilder();
    }
    public static Builder newBuilder(io.tortuga.TortugaParamsProto.TortugaBridgeOpts prototype) {
      return DEFAULT_INSTANCE.toBuilder().mergeFrom(prototype);
    }
    public Builder toBuilder() {
      return this == DEFAULT_INSTANCE
          ? new Builder() : new Builder().mergeFrom(this);
    }

    @java.lang.Override
    protected Builder newBuilderForType(
        com.google.protobuf.GeneratedMessageV3.BuilderParent parent) {
      Builder builder = new Builder(parent);
      return builder;
    }
    /**
     * Protobuf type {@code tortuga.TortugaBridgeOpts}
     */
    public static final class Builder extends
        com.google.protobuf.GeneratedMessageV3.Builder<Builder> implements
        // @@protoc_insertion_point(builder_implements:tortuga.TortugaBridgeOpts)
        io.tortuga.TortugaParamsProto.TortugaBridgeOptsOrBuilder {
      public static final com.google.protobuf.Descriptors.Descriptor
          getDescriptor() {
        return io.tortuga.TortugaParamsProto.internal_static_tortuga_TortugaBridgeOpts_descriptor;
      }

      protected com.google.protobuf.GeneratedMessageV3.FieldAccessorTable
          internalGetFieldAccessorTable() {
        return io.tortuga.TortugaParamsProto.internal_static_tortuga_TortugaBridgeOpts_fieldAccessorTable
            .ensureFieldAccessorsInitialized(
                io.tortuga.TortugaParamsProto.TortugaBridgeOpts.class, io.tortuga.TortugaParamsProto.TortugaBridgeOpts.Builder.class);
      }

      // Construct using io.tortuga.TortugaParamsProto.TortugaBridgeOpts.newBuilder()
      private Builder() {
        maybeForceBuilderInitialization();
      }

      private Builder(
          com.google.protobuf.GeneratedMessageV3.BuilderParent parent) {
        super(parent);
        maybeForceBuilderInitialization();
      }
      private void maybeForceBuilderInitialization() {
        if (com.google.protobuf.GeneratedMessageV3
                .alwaysUseFieldBuilders) {
        }
      }
      public Builder clear() {
        super.clear();
        timeoutSeconds_ = 0;
        bitField0_ = (bitField0_ & ~0x00000001);
        return this;
      }

      public com.google.protobuf.Descriptors.Descriptor
          getDescriptorForType() {
        return io.tortuga.TortugaParamsProto.internal_static_tortuga_TortugaBridgeOpts_descriptor;
      }

      public io.tortuga.TortugaParamsProto.TortugaBridgeOpts getDefaultInstanceForType() {
        return io.tortuga.TortugaParamsProto.TortugaBridgeOpts.getDefaultInstance();
      }

      public io.tortuga.TortugaParamsProto.TortugaBridgeOpts build() {
        io.tortuga.TortugaParamsProto.TortugaBridgeOpts result = buildPartial();
        if (!result.isInitialized()) {
          throw newUninitializedMessageException(result);
        }
        return result;
      }

      public io.tortuga.TortugaParamsProto.TortugaBridgeOpts buildPartial() {
        io.tortuga.TortugaParamsProto.TortugaBridgeOpts result = new io.tortuga.TortugaParamsProto.TortugaBridgeOpts(this);
        int from_bitField0_ = bitField0_;
        int to_bitField0_ = 0;
        if (((from_bitField0_ & 0x00000001) == 0x00000001)) {
          to_bitField0_ |= 0x00000001;
        }
        result.timeoutSeconds_ = timeoutSeconds_;
        result.bitField0_ = to_bitField0_;
        onBuilt();
        return result;
      }

      public Builder clone() {
        return (Builder) super.clone();
      }
      public Builder setField(
          com.google.protobuf.Descriptors.FieldDescriptor field,
          java.lang.Object value) {
        return (Builder) super.setField(field, value);
      }
      public Builder clearField(
          com.google.protobuf.Descriptors.FieldDescriptor field) {
        return (Builder) super.clearField(field);
      }
      public Builder clearOneof(
          com.google.protobuf.Descriptors.OneofDescriptor oneof) {
        return (Builder) super.clearOneof(oneof);
      }
      public Builder setRepeatedField(
          com.google.protobuf.Descriptors.FieldDescriptor field,
          int index, java.lang.Object value) {
        return (Builder) super.setRepeatedField(field, index, value);
      }
      public Builder addRepeatedField(
          com.google.protobuf.Descriptors.FieldDescriptor field,
          java.lang.Object value) {
        return (Builder) super.addRepeatedField(field, value);
      }
      public Builder mergeFrom(com.google.protobuf.Message other) {
        if (other instanceof io.tortuga.TortugaParamsProto.TortugaBridgeOpts) {
          return mergeFrom((io.tortuga.TortugaParamsProto.TortugaBridgeOpts)other);
        } else {
          super.mergeFrom(other);
          return this;
        }
      }

      public Builder mergeFrom(io.tortuga.TortugaParamsProto.TortugaBridgeOpts other) {
        if (other == io.tortuga.TortugaParamsProto.TortugaBridgeOpts.getDefaultInstance()) return this;
        if (other.hasTimeoutSeconds()) {
          setTimeoutSeconds(other.getTimeoutSeconds());
        }
        this.mergeUnknownFields(other.unknownFields);
        onChanged();
        return this;
      }

      public final boolean isInitialized() {
        return true;
      }

      public Builder mergeFrom(
          com.google.protobuf.CodedInputStream input,
          com.google.protobuf.ExtensionRegistryLite extensionRegistry)
          throws java.io.IOException {
        io.tortuga.TortugaParamsProto.TortugaBridgeOpts parsedMessage = null;
        try {
          parsedMessage = PARSER.parsePartialFrom(input, extensionRegistry);
        } catch (com.google.protobuf.InvalidProtocolBufferException e) {
          parsedMessage = (io.tortuga.TortugaParamsProto.TortugaBridgeOpts) e.getUnfinishedMessage();
          throw e.unwrapIOException();
        } finally {
          if (parsedMessage != null) {
            mergeFrom(parsedMessage);
          }
        }
        return this;
      }
      private int bitField0_;

      private int timeoutSeconds_ ;
      /**
       * <code>optional int32 timeout_seconds = 1;</code>
       */
      public boolean hasTimeoutSeconds() {
        return ((bitField0_ & 0x00000001) == 0x00000001);
      }
      /**
       * <code>optional int32 timeout_seconds = 1;</code>
       */
      public int getTimeoutSeconds() {
        return timeoutSeconds_;
      }
      /**
       * <code>optional int32 timeout_seconds = 1;</code>
       */
      public Builder setTimeoutSeconds(int value) {
        bitField0_ |= 0x00000001;
        timeoutSeconds_ = value;
        onChanged();
        return this;
      }
      /**
       * <code>optional int32 timeout_seconds = 1;</code>
       */
      public Builder clearTimeoutSeconds() {
        bitField0_ = (bitField0_ & ~0x00000001);
        timeoutSeconds_ = 0;
        onChanged();
        return this;
      }
      public final Builder setUnknownFields(
          final com.google.protobuf.UnknownFieldSet unknownFields) {
        return super.setUnknownFields(unknownFields);
      }

      public final Builder mergeUnknownFields(
          final com.google.protobuf.UnknownFieldSet unknownFields) {
        return super.mergeUnknownFields(unknownFields);
      }


      // @@protoc_insertion_point(builder_scope:tortuga.TortugaBridgeOpts)
    }

    // @@protoc_insertion_point(class_scope:tortuga.TortugaBridgeOpts)
    private static final io.tortuga.TortugaParamsProto.TortugaBridgeOpts DEFAULT_INSTANCE;
    static {
      DEFAULT_INSTANCE = new io.tortuga.TortugaParamsProto.TortugaBridgeOpts();
    }

    public static io.tortuga.TortugaParamsProto.TortugaBridgeOpts getDefaultInstance() {
      return DEFAULT_INSTANCE;
    }

    @java.lang.Deprecated public static final com.google.protobuf.Parser<TortugaBridgeOpts>
        PARSER = new com.google.protobuf.AbstractParser<TortugaBridgeOpts>() {
      public TortugaBridgeOpts parsePartialFrom(
          com.google.protobuf.CodedInputStream input,
          com.google.protobuf.ExtensionRegistryLite extensionRegistry)
          throws com.google.protobuf.InvalidProtocolBufferException {
        return new TortugaBridgeOpts(input, extensionRegistry);
      }
    };

    public static com.google.protobuf.Parser<TortugaBridgeOpts> parser() {
      return PARSER;
    }

    @java.lang.Override
    public com.google.protobuf.Parser<TortugaBridgeOpts> getParserForType() {
      return PARSER;
    }

    public io.tortuga.TortugaParamsProto.TortugaBridgeOpts getDefaultInstanceForType() {
      return DEFAULT_INSTANCE;
    }

  }

  public static final int BRIDGE_FIELD_NUMBER = 50007;
  /**
   * <code>extend .google.protobuf.MethodOptions { ... }</code>
   */
  public static final
    com.google.protobuf.GeneratedMessage.GeneratedExtension<
      com.google.protobuf.DescriptorProtos.MethodOptions,
      io.tortuga.TortugaParamsProto.TortugaBridgeOpts> bridge = com.google.protobuf.GeneratedMessage
          .newFileScopedGeneratedExtension(
        io.tortuga.TortugaParamsProto.TortugaBridgeOpts.class,
        io.tortuga.TortugaParamsProto.TortugaBridgeOpts.getDefaultInstance());
  private static final com.google.protobuf.Descriptors.Descriptor
    internal_static_tortuga_TortugaBridgeOpts_descriptor;
  private static final 
    com.google.protobuf.GeneratedMessageV3.FieldAccessorTable
      internal_static_tortuga_TortugaBridgeOpts_fieldAccessorTable;

  public static com.google.protobuf.Descriptors.FileDescriptor
      getDescriptor() {
    return descriptor;
  }
  private static  com.google.protobuf.Descriptors.FileDescriptor
      descriptor;
  static {
    java.lang.String[] descriptorData = {
      "\n\034tortuga/tortuga_params.proto\022\007tortuga\032" +
      " google/protobuf/descriptor.proto\",\n\021Tor" +
      "tugaBridgeOpts\022\027\n\017timeout_seconds\030\001 \001(\005:" +
      "L\n\006bridge\022\036.google.protobuf.MethodOption" +
      "s\030\327\206\003 \001(\0132\032.tortuga.TortugaBridgeOptsB \n" +
      "\nio.tortugaB\022TortugaParamsProto"
    };
    com.google.protobuf.Descriptors.FileDescriptor.InternalDescriptorAssigner assigner =
        new com.google.protobuf.Descriptors.FileDescriptor.    InternalDescriptorAssigner() {
          public com.google.protobuf.ExtensionRegistry assignDescriptors(
              com.google.protobuf.Descriptors.FileDescriptor root) {
            descriptor = root;
            return null;
          }
        };
    com.google.protobuf.Descriptors.FileDescriptor
      .internalBuildGeneratedFileFrom(descriptorData,
        new com.google.protobuf.Descriptors.FileDescriptor[] {
          com.google.protobuf.DescriptorProtos.getDescriptor(),
        }, assigner);
    internal_static_tortuga_TortugaBridgeOpts_descriptor =
      getDescriptor().getMessageTypes().get(0);
    internal_static_tortuga_TortugaBridgeOpts_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessageV3.FieldAccessorTable(
        internal_static_tortuga_TortugaBridgeOpts_descriptor,
        new java.lang.String[] { "TimeoutSeconds", });
    bridge.internalInit(descriptor.getExtensions().get(0));
    com.google.protobuf.DescriptorProtos.getDescriptor();
  }

  // @@protoc_insertion_point(outer_class_scope)
}