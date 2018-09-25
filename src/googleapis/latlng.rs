// This file is generated by rust-protobuf 2.0.5. Do not edit
// @generated

// https://github.com/Manishearth/rust-clippy/issues/702
#![allow(unknown_lints)]
#![allow(clippy)]

#![cfg_attr(rustfmt, rustfmt_skip)]

#![allow(box_pointers)]
#![allow(dead_code)]
#![allow(missing_docs)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(non_upper_case_globals)]
#![allow(trivial_casts)]
#![allow(unsafe_code)]
#![allow(unused_imports)]
#![allow(unused_results)]

use protobuf::Message as Message_imported_for_functions;
use protobuf::ProtobufEnum as ProtobufEnum_imported_for_functions;

#[derive(PartialEq,Clone,Default)]
pub struct LatLng {
    // message fields
    pub latitude: f64,
    pub longitude: f64,
    // special fields
    unknown_fields: ::protobuf::UnknownFields,
    cached_size: ::protobuf::CachedSize,
}

impl LatLng {
    pub fn new() -> LatLng {
        ::std::default::Default::default()
    }

    // double latitude = 1;

    pub fn clear_latitude(&mut self) {
        self.latitude = 0.;
    }

    // Param is passed by value, moved
    pub fn set_latitude(&mut self, v: f64) {
        self.latitude = v;
    }

    pub fn get_latitude(&self) -> f64 {
        self.latitude
    }

    // double longitude = 2;

    pub fn clear_longitude(&mut self) {
        self.longitude = 0.;
    }

    // Param is passed by value, moved
    pub fn set_longitude(&mut self, v: f64) {
        self.longitude = v;
    }

    pub fn get_longitude(&self) -> f64 {
        self.longitude
    }
}

impl ::protobuf::Message for LatLng {
    fn is_initialized(&self) -> bool {
        true
    }

    fn merge_from(&mut self, is: &mut ::protobuf::CodedInputStream) -> ::protobuf::ProtobufResult<()> {
        while !is.eof()? {
            let (field_number, wire_type) = is.read_tag_unpack()?;
            match field_number {
                1 => {
                    if wire_type != ::protobuf::wire_format::WireTypeFixed64 {
                        return ::std::result::Result::Err(::protobuf::rt::unexpected_wire_type(wire_type));
                    }
                    let tmp = is.read_double()?;
                    self.latitude = tmp;
                },
                2 => {
                    if wire_type != ::protobuf::wire_format::WireTypeFixed64 {
                        return ::std::result::Result::Err(::protobuf::rt::unexpected_wire_type(wire_type));
                    }
                    let tmp = is.read_double()?;
                    self.longitude = tmp;
                },
                _ => {
                    ::protobuf::rt::read_unknown_or_skip_group(field_number, wire_type, is, self.mut_unknown_fields())?;
                },
            };
        }
        ::std::result::Result::Ok(())
    }

    // Compute sizes of nested messages
    #[allow(unused_variables)]
    fn compute_size(&self) -> u32 {
        let mut my_size = 0;
        if self.latitude != 0. {
            my_size += 9;
        }
        if self.longitude != 0. {
            my_size += 9;
        }
        my_size += ::protobuf::rt::unknown_fields_size(self.get_unknown_fields());
        self.cached_size.set(my_size);
        my_size
    }

    fn write_to_with_cached_sizes(&self, os: &mut ::protobuf::CodedOutputStream) -> ::protobuf::ProtobufResult<()> {
        if self.latitude != 0. {
            os.write_double(1, self.latitude)?;
        }
        if self.longitude != 0. {
            os.write_double(2, self.longitude)?;
        }
        os.write_unknown_fields(self.get_unknown_fields())?;
        ::std::result::Result::Ok(())
    }

    fn get_cached_size(&self) -> u32 {
        self.cached_size.get()
    }

    fn get_unknown_fields(&self) -> &::protobuf::UnknownFields {
        &self.unknown_fields
    }

    fn mut_unknown_fields(&mut self) -> &mut ::protobuf::UnknownFields {
        &mut self.unknown_fields
    }

    fn as_any(&self) -> &::std::any::Any {
        self as &::std::any::Any
    }
    fn as_any_mut(&mut self) -> &mut ::std::any::Any {
        self as &mut ::std::any::Any
    }
    fn into_any(self: Box<Self>) -> ::std::boxed::Box<::std::any::Any> {
        self
    }

    fn descriptor(&self) -> &'static ::protobuf::reflect::MessageDescriptor {
        Self::descriptor_static()
    }

    fn new() -> LatLng {
        LatLng::new()
    }

    fn descriptor_static() -> &'static ::protobuf::reflect::MessageDescriptor {
        static mut descriptor: ::protobuf::lazy::Lazy<::protobuf::reflect::MessageDescriptor> = ::protobuf::lazy::Lazy {
            lock: ::protobuf::lazy::ONCE_INIT,
            ptr: 0 as *const ::protobuf::reflect::MessageDescriptor,
        };
        unsafe {
            descriptor.get(|| {
                let mut fields = ::std::vec::Vec::new();
                fields.push(::protobuf::reflect::accessor::make_simple_field_accessor::<_, ::protobuf::types::ProtobufTypeDouble>(
                    "latitude",
                    |m: &LatLng| { &m.latitude },
                    |m: &mut LatLng| { &mut m.latitude },
                ));
                fields.push(::protobuf::reflect::accessor::make_simple_field_accessor::<_, ::protobuf::types::ProtobufTypeDouble>(
                    "longitude",
                    |m: &LatLng| { &m.longitude },
                    |m: &mut LatLng| { &mut m.longitude },
                ));
                ::protobuf::reflect::MessageDescriptor::new::<LatLng>(
                    "LatLng",
                    fields,
                    file_descriptor_proto()
                )
            })
        }
    }

    fn default_instance() -> &'static LatLng {
        static mut instance: ::protobuf::lazy::Lazy<LatLng> = ::protobuf::lazy::Lazy {
            lock: ::protobuf::lazy::ONCE_INIT,
            ptr: 0 as *const LatLng,
        };
        unsafe {
            instance.get(LatLng::new)
        }
    }
}

impl ::protobuf::Clear for LatLng {
    fn clear(&mut self) {
        self.clear_latitude();
        self.clear_longitude();
        self.unknown_fields.clear();
    }
}

impl ::std::fmt::Debug for LatLng {
    fn fmt(&self, f: &mut ::std::fmt::Formatter) -> ::std::fmt::Result {
        ::protobuf::text_format::fmt(self, f)
    }
}

impl ::protobuf::reflect::ProtobufValue for LatLng {
    fn as_ref(&self) -> ::protobuf::reflect::ProtobufValueRef {
        ::protobuf::reflect::ProtobufValueRef::Message(self)
    }
}

static file_descriptor_proto_data: &'static [u8] = b"\
    \n\x18google/type/latlng.proto\x12\x0bgoogle.type\"B\n\x06LatLng\x12\x1a\
    \n\x08latitude\x18\x01\x20\x01(\x01R\x08latitude\x12\x1c\n\tlongitude\
    \x18\x02\x20\x01(\x01R\tlongitudeB`\n\x0fcom.google.typeB\x0bLatLngProto\
    P\x01Z8google.golang.org/genproto/googleapis/type/latlng;latlng\xa2\x02\
    \x03GTPJ\xef\x17\n\x06\x12\x04\x0e\0F\x01\n\xbd\x04\n\x01\x0c\x12\x03\
    \x0e\0\x122\xb2\x04\x20Copyright\x202016\x20Google\x20Inc.\n\n\x20Licens\
    ed\x20under\x20the\x20Apache\x20License,\x20Version\x202.0\x20(the\x20\"\
    License\");\n\x20you\x20may\x20not\x20use\x20this\x20file\x20except\x20i\
    n\x20compliance\x20with\x20the\x20License.\n\x20You\x20may\x20obtain\x20\
    a\x20copy\x20of\x20the\x20License\x20at\n\n\x20\x20\x20\x20\x20http://ww\
    w.apache.org/licenses/LICENSE-2.0\n\n\x20Unless\x20required\x20by\x20app\
    licable\x20law\x20or\x20agreed\x20to\x20in\x20writing,\x20software\n\x20\
    distributed\x20under\x20the\x20License\x20is\x20distributed\x20on\x20an\
    \x20\"AS\x20IS\"\x20BASIS,\n\x20WITHOUT\x20WARRANTIES\x20OR\x20CONDITION\
    S\x20OF\x20ANY\x20KIND,\x20either\x20express\x20or\x20implied.\n\x20See\
    \x20the\x20License\x20for\x20the\x20specific\x20language\x20governing\
    \x20permissions\x20and\n\x20limitations\x20under\x20the\x20License.\n\n\
    \x08\n\x01\x02\x12\x03\x10\x08\x13\n\x08\n\x01\x08\x12\x03\x12\0O\n\x0b\
    \n\x04\x08\xe7\x07\0\x12\x03\x12\0O\n\x0c\n\x05\x08\xe7\x07\0\x02\x12\
    \x03\x12\x07\x11\n\r\n\x06\x08\xe7\x07\0\x02\0\x12\x03\x12\x07\x11\n\x0e\
    \n\x07\x08\xe7\x07\0\x02\0\x01\x12\x03\x12\x07\x11\n\x0c\n\x05\x08\xe7\
    \x07\0\x07\x12\x03\x12\x14N\n\x08\n\x01\x08\x12\x03\x13\0\"\n\x0b\n\x04\
    \x08\xe7\x07\x01\x12\x03\x13\0\"\n\x0c\n\x05\x08\xe7\x07\x01\x02\x12\x03\
    \x13\x07\x1a\n\r\n\x06\x08\xe7\x07\x01\x02\0\x12\x03\x13\x07\x1a\n\x0e\n\
    \x07\x08\xe7\x07\x01\x02\0\x01\x12\x03\x13\x07\x1a\n\x0c\n\x05\x08\xe7\
    \x07\x01\x03\x12\x03\x13\x1d!\n\x08\n\x01\x08\x12\x03\x14\0,\n\x0b\n\x04\
    \x08\xe7\x07\x02\x12\x03\x14\0,\n\x0c\n\x05\x08\xe7\x07\x02\x02\x12\x03\
    \x14\x07\x1b\n\r\n\x06\x08\xe7\x07\x02\x02\0\x12\x03\x14\x07\x1b\n\x0e\n\
    \x07\x08\xe7\x07\x02\x02\0\x01\x12\x03\x14\x07\x1b\n\x0c\n\x05\x08\xe7\
    \x07\x02\x07\x12\x03\x14\x1e+\n\x08\n\x01\x08\x12\x03\x15\0(\n\x0b\n\x04\
    \x08\xe7\x07\x03\x12\x03\x15\0(\n\x0c\n\x05\x08\xe7\x07\x03\x02\x12\x03\
    \x15\x07\x13\n\r\n\x06\x08\xe7\x07\x03\x02\0\x12\x03\x15\x07\x13\n\x0e\n\
    \x07\x08\xe7\x07\x03\x02\0\x01\x12\x03\x15\x07\x13\n\x0c\n\x05\x08\xe7\
    \x07\x03\x07\x12\x03\x15\x16'\n\x08\n\x01\x08\x12\x03\x16\0!\n\x0b\n\x04\
    \x08\xe7\x07\x04\x12\x03\x16\0!\n\x0c\n\x05\x08\xe7\x07\x04\x02\x12\x03\
    \x16\x07\x18\n\r\n\x06\x08\xe7\x07\x04\x02\0\x12\x03\x16\x07\x18\n\x0e\n\
    \x07\x08\xe7\x07\x04\x02\0\x01\x12\x03\x16\x07\x18\n\x0c\n\x05\x08\xe7\
    \x07\x04\x07\x12\x03\x16\x1b\x20\n\xdd\r\n\x02\x04\0\x12\x04@\0F\x01\x1a\
    \xd0\r\x20An\x20object\x20representing\x20a\x20latitude/longitude\x20pai\
    r.\x20This\x20is\x20expressed\x20as\x20a\x20pair\n\x20of\x20doubles\x20r\
    epresenting\x20degrees\x20latitude\x20and\x20degrees\x20longitude.\x20Un\
    less\n\x20specified\x20otherwise,\x20this\x20must\x20conform\x20to\x20th\
    e\n\x20<a\x20href=\"http://www.unoosa.org/pdf/icg/2012/template/WGS_84.p\
    df\">WGS84\n\x20standard</a>.\x20Values\x20must\x20be\x20within\x20norma\
    lized\x20ranges.\n\n\x20Example\x20of\x20normalization\x20code\x20in\x20\
    Python:\n\n\x20\x20\x20\x20\x20def\x20NormalizeLongitude(longitude):\n\
    \x20\x20\x20\x20\x20\x20\x20\"\"\"Wraps\x20decimal\x20degrees\x20longitu\
    de\x20to\x20[-180.0,\x20180.0].\"\"\"\n\x20\x20\x20\x20\x20\x20\x20q,\
    \x20r\x20=\x20divmod(longitude,\x20360.0)\n\x20\x20\x20\x20\x20\x20\x20i\
    f\x20r\x20>\x20180.0\x20or\x20(r\x20==\x20180.0\x20and\x20q\x20<=\x20-1.\
    0):\n\x20\x20\x20\x20\x20\x20\x20\x20\x20return\x20r\x20-\x20360.0\n\x20\
    \x20\x20\x20\x20\x20\x20return\x20r\n\n\x20\x20\x20\x20\x20def\x20Normal\
    izeLatLng(latitude,\x20longitude):\n\x20\x20\x20\x20\x20\x20\x20\"\"\"Wr\
    aps\x20decimal\x20degrees\x20latitude\x20and\x20longitude\x20to\n\x20\
    \x20\x20\x20\x20\x20\x20[-90.0,\x2090.0]\x20and\x20[-180.0,\x20180.0],\
    \x20respectively.\"\"\"\n\x20\x20\x20\x20\x20\x20\x20r\x20=\x20latitude\
    \x20%\x20360.0\n\x20\x20\x20\x20\x20\x20\x20if\x20r\x20<=\x2090.0:\n\x20\
    \x20\x20\x20\x20\x20\x20\x20\x20return\x20r,\x20NormalizeLongitude(longi\
    tude)\n\x20\x20\x20\x20\x20\x20\x20elif\x20r\x20>=\x20270.0:\n\x20\x20\
    \x20\x20\x20\x20\x20\x20\x20return\x20r\x20-\x20360,\x20NormalizeLongitu\
    de(longitude)\n\x20\x20\x20\x20\x20\x20\x20else:\n\x20\x20\x20\x20\x20\
    \x20\x20\x20\x20return\x20180\x20-\x20r,\x20NormalizeLongitude(longitude\
    \x20+\x20180.0)\n\n\x20\x20\x20\x20\x20assert\x20180.0\x20==\x20Normaliz\
    eLongitude(180.0)\n\x20\x20\x20\x20\x20assert\x20-180.0\x20==\x20Normali\
    zeLongitude(-180.0)\n\x20\x20\x20\x20\x20assert\x20-179.0\x20==\x20Norma\
    lizeLongitude(181.0)\n\x20\x20\x20\x20\x20assert\x20(0.0,\x200.0)\x20==\
    \x20NormalizeLatLng(360.0,\x200.0)\n\x20\x20\x20\x20\x20assert\x20(0.0,\
    \x200.0)\x20==\x20NormalizeLatLng(-360.0,\x200.0)\n\x20\x20\x20\x20\x20a\
    ssert\x20(85.0,\x20180.0)\x20==\x20NormalizeLatLng(95.0,\x200.0)\n\x20\
    \x20\x20\x20\x20assert\x20(-85.0,\x20-170.0)\x20==\x20NormalizeLatLng(-9\
    5.0,\x2010.0)\n\x20\x20\x20\x20\x20assert\x20(90.0,\x2010.0)\x20==\x20No\
    rmalizeLatLng(90.0,\x2010.0)\n\x20\x20\x20\x20\x20assert\x20(-90.0,\x20-\
    10.0)\x20==\x20NormalizeLatLng(-90.0,\x20-10.0)\n\x20\x20\x20\x20\x20ass\
    ert\x20(0.0,\x20-170.0)\x20==\x20NormalizeLatLng(-180.0,\x2010.0)\n\x20\
    \x20\x20\x20\x20assert\x20(0.0,\x20-170.0)\x20==\x20NormalizeLatLng(180.\
    0,\x2010.0)\n\x20\x20\x20\x20\x20assert\x20(-90.0,\x2010.0)\x20==\x20Nor\
    malizeLatLng(270.0,\x2010.0)\n\x20\x20\x20\x20\x20assert\x20(90.0,\x2010\
    .0)\x20==\x20NormalizeLatLng(-270.0,\x2010.0)\n\n\n\n\x03\x04\0\x01\x12\
    \x03@\x08\x0e\nO\n\x04\x04\0\x02\0\x12\x03B\x02\x16\x1aB\x20The\x20latit\
    ude\x20in\x20degrees.\x20It\x20must\x20be\x20in\x20the\x20range\x20[-90.\
    0,\x20+90.0].\n\n\r\n\x05\x04\0\x02\0\x04\x12\x04B\x02@\x10\n\x0c\n\x05\
    \x04\0\x02\0\x05\x12\x03B\x02\x08\n\x0c\n\x05\x04\0\x02\0\x01\x12\x03B\t\
    \x11\n\x0c\n\x05\x04\0\x02\0\x03\x12\x03B\x14\x15\nR\n\x04\x04\0\x02\x01\
    \x12\x03E\x02\x17\x1aE\x20The\x20longitude\x20in\x20degrees.\x20It\x20mu\
    st\x20be\x20in\x20the\x20range\x20[-180.0,\x20+180.0].\n\n\r\n\x05\x04\0\
    \x02\x01\x04\x12\x04E\x02B\x16\n\x0c\n\x05\x04\0\x02\x01\x05\x12\x03E\
    \x02\x08\n\x0c\n\x05\x04\0\x02\x01\x01\x12\x03E\t\x12\n\x0c\n\x05\x04\0\
    \x02\x01\x03\x12\x03E\x15\x16b\x06proto3\
";

static mut file_descriptor_proto_lazy: ::protobuf::lazy::Lazy<::protobuf::descriptor::FileDescriptorProto> = ::protobuf::lazy::Lazy {
    lock: ::protobuf::lazy::ONCE_INIT,
    ptr: 0 as *const ::protobuf::descriptor::FileDescriptorProto,
};

fn parse_descriptor_proto() -> ::protobuf::descriptor::FileDescriptorProto {
    ::protobuf::parse_from_bytes(file_descriptor_proto_data).unwrap()
}

pub fn file_descriptor_proto() -> &'static ::protobuf::descriptor::FileDescriptorProto {
    unsafe {
        file_descriptor_proto_lazy.get(|| {
            parse_descriptor_proto()
        })
    }
}
