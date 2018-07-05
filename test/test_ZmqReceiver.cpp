#include "../src/ZmqReceiver.hpp"

using namespace std;

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