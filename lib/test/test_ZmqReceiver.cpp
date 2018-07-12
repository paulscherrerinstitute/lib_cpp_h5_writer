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
  HeaderDataType header_data_type_scalar("uint64", 1);
  
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