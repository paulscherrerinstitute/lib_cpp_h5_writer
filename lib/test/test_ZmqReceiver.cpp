#include "../src/ZmqReceiver.hpp"
#include "../src/ZmqReceiver.hpp"

using namespace std;
namespace pt = boost::property_tree;

TEST(ZmqReceiver, get_type_byte_size)
{ 
  EXPECT_TRUE(get_type_byte_size("uint8") == 1);	
  EXPECT_TRUE(get_type_byte_size("int8") == 1);
  EXPECT_TRUE(get_type_byte_size("uint16") == 2);
  EXPECT_TRUE(get_type_byte_size("int16") == 2);
  EXPECT_TRUE(get_type_byte_size("uint32") == 4);
  EXPECT_TRUE(get_type_byte_size("int32") == 4);
  EXPECT_TRUE(get_type_byte_size("float32") == 4);
  EXPECT_TRUE(get_type_byte_size("uint64") == 8);
  EXPECT_TRUE(get_type_byte_size("int64") == 8);
  EXPECT_TRUE(get_type_byte_size("float64") == 8);
}

TEST(ZmqReceiver, HeaderDataType)
{
  HeaderDataType header_data_type("float64", 4);

  ASSERT_TRUE(header_data_type.type == "float64");
  ASSERT_TRUE(header_data_type.value_bytes_size == 8);
  ASSERT_TRUE(header_data_type.value_shape == 4);
  ASSERT_TRUE(header_data_type.endianness == "little");
}

TEST(ZmqReceiver, get_value_from_json)
{
  pt::ptree json_header;

  uint64_t frame_number = 1234567890;
  json_header.add("frame_number", frame_number);
  HeaderDataType header_data_type_scalar("uint64");
  
  auto scalar_buffer = get_value_from_json(json_header, "frame_number", header_data_type_scalar);
  auto scalar_value = reinterpret_cast<uint64_t*>(scalar_buffer.get());
  
  ASSERT_TRUE(*scalar_value == frame_number);

  double modules_number[] = {-345.12, 1234567.43, -2323456.32};
  pt::ptree modulus_number_child;

  for (int i=0; i<3; i++) {
    pt::ptree value;
    value.put("", modules_number[i]);
    modulus_number_child.push_back(make_pair("", value));
  }

  json_header.add_child("modules_number", modulus_number_child);

  HeaderDataType header_data_type_array("float64", 3);
  
  auto array_buffer = get_value_from_json(json_header, "modules_number", header_data_type_array);
  auto array_values = reinterpret_cast<double*>(array_buffer.get());

  for (int i=0; i<3; i++) {
    ASSERT_TRUE(array_values[i] == modules_number[i]);
  }
}

TEST(ZmqReceiver, read_json_header)
{
  int n_modules = 1;

  auto header_values = shared_ptr<unordered_map<string, HeaderDataType>>(new unordered_map<string, HeaderDataType> {
      {"pulse_id", HeaderDataType("uint64")},
      {"frame", HeaderDataType("uint64")},
      {"is_good_frame", HeaderDataType("uint64")},
      {"daq_rec", HeaderDataType("int64")},

      {"pulse_id_diff", HeaderDataType("int64", n_modules)},
      {"framenum_diff", HeaderDataType("int64", n_modules)},

      {"missing_packets_1", HeaderDataType("uint64", n_modules)},
      {"missing_packets_2", HeaderDataType("uint64", n_modules)},
      {"daq_recs", HeaderDataType("uint64", n_modules)},
      
      {"pulse_ids", HeaderDataType("uint64", n_modules)},
      {"framenums", HeaderDataType("uint64", n_modules)},
      
      {"module_number", HeaderDataType("uint64", n_modules)}
  });

  ZmqReceiver receiver("something", 1, 1, header_values);

  auto header_string = "{\"missing_packets_2\":[0],\"missing_packets_1\":[0],\"frame\":0,\"daq_recs\":[3840],\"module_number\":[0],\"shape\":[512,1024],\"pulse_id\":6021771850,\"framenum_diff\":[0],\"pulse_ids\":[6021771850],\"is_good_frame\":1,\"framenums\":[193],\"pulse_id_diff\":[0],\"daq_rec\":3840,\"type\":\"uint16\",\"htype\":\"array-1.0\"}";

  auto frame_metadata = receiver.read_json_header(header_string);
}