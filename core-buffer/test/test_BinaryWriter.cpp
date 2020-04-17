#include <BinaryWriter.hpp>
#include "gtest/gtest.h"

TEST(BinaryWriter, basic_interaction)
{
    auto device_name = "test_device";
    auto root_folder = ".";

    BinaryWriter writer(device_name, root_folder);

    JFFileFormat frame_data;
    uint64_t pulse_id = 1;
    uint64_t frame_id = 2;
    uint32_t daq_rec = 3;
    uint16_t n_recv_packets = 4;

    ASSERT_EQ(frame_data.FORMAT_MARKER, JF_FORMAT_START_BYTE);

    writer.write(5, &frame_data);
}
