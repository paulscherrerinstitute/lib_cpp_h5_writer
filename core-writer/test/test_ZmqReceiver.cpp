#include <thread>
#include "gtest/gtest.h"
#include "ZmqReceiver.hpp"
#include "mock/stream.hpp"

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

  unordered_map<string, HeaderDataType> header_values = {
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
  };

  ZmqReceiver receiver(header_values);

  auto header_string = "{\"missing_packets_2\":[2],"
                        "\"missing_packets_1\":[1],"
                        "\"frame\":0,"
                        "\"daq_recs\":[3840],"
                        "\"module_number\":[0],"
                        "\"shape\":[512,1024],"
                        "\"pulse_id\":6021771850,"
                        "\"framenum_diff\":[-2],"
                        "\"pulse_ids\":[6021771850],"
                        "\"is_good_frame\":1,"
                        "\"framenums\":[193],"
                        "\"pulse_id_diff\":[-1],"
                        "\"daq_rec\":-1,"
                        "\"type\":\"uint16\","
                        "\"htype\":\"array-1.0\"}";

  auto metadata = receiver.read_json_header(header_string);

  ASSERT_TRUE(metadata->frame_index == 0);
  ASSERT_TRUE(metadata->endianness == "little");
  ASSERT_TRUE(metadata->type == "uint16");
  ASSERT_TRUE(metadata->frame_shape[0] == 512);
  ASSERT_TRUE(metadata->frame_shape[1] == 1024);

  auto pulse_id = reinterpret_cast<uint64_t*>(metadata->header_values.at("pulse_id").get());
  ASSERT_TRUE(pulse_id[0] == 6021771850);

  auto frame = reinterpret_cast<uint64_t*>(metadata->header_values.at("frame").get());
  ASSERT_TRUE(frame[0] == 0);

  auto is_good_frame = reinterpret_cast<uint64_t*>(metadata->header_values.at("is_good_frame").get());
  ASSERT_TRUE(is_good_frame[0] == 1);

  auto daq_rec = reinterpret_cast<int64_t*>(metadata->header_values.at("daq_rec").get());
  ASSERT_TRUE(daq_rec[0] == -1);

  auto pulse_id_diff = reinterpret_cast<int64_t*>(metadata->header_values.at("pulse_id_diff").get());
  ASSERT_TRUE(pulse_id_diff[0] == -1);

  auto framenum_diff = reinterpret_cast<int64_t*>(metadata->header_values.at("framenum_diff").get());
  ASSERT_TRUE(framenum_diff[0] == -2);

  auto missing_packets_1 = reinterpret_cast<uint64_t*>(metadata->header_values.at("missing_packets_1").get());
  ASSERT_TRUE(missing_packets_1[0] == 1);

  auto missing_packets_2 = reinterpret_cast<uint64_t*>(metadata->header_values.at("missing_packets_2").get());
  ASSERT_TRUE(missing_packets_2[0] == 2);

  auto daq_recs = reinterpret_cast<uint64_t*>(metadata->header_values.at("daq_recs").get());
  ASSERT_TRUE(daq_recs[0] == 3840);

  auto pulse_ids = reinterpret_cast<uint64_t*>(metadata->header_values.at("pulse_ids").get());
  ASSERT_TRUE(pulse_ids[0] == 6021771850);

  auto framenums = reinterpret_cast<uint64_t*>(metadata->header_values.at("framenums").get());
  ASSERT_TRUE(framenums[0] == 193);

  auto module_number = reinterpret_cast<uint64_t*>(metadata->header_values.at("module_number").get());
  ASSERT_TRUE(module_number[0] == 0);
}

TEST(ZmqReceiver, simple_recv)
{
    size_t n_msg = 10;

    thread sender(generate_stream, n_msg);
    RingBuffer<FrameMetadata> ring_buffer(n_msg);

    ZmqReceiver receiver({});
    receiver.connect(MOCK_STREAM_ADDRESS, 100);

    this_thread::sleep_for(chrono::milliseconds(100));

    for (size_t i=0; i<n_msg; i++) {
        EXPECT_TRUE(receiver.receive().first != nullptr);
    }

    sender.join();
}