#include "FastH5Writer.hpp"
#include "gtest/gtest.h"

TEST(FastH5Writer, basic_interaction)
{
    auto root_folder = ".";
    auto device_name = "fast_device";

    auto buffer = make_shared<char[]>(512*1024*2);

    UdpFrameMetadata metadata;
    metadata.pulse_id = 1;
    metadata.frame_index = 2;
    metadata.daq_rec = 2;
    metadata.n_recv_packets = 128;

    FastH5Writer writer(
            BufferUtils::FILE_MOD, 512, 1024, device_name, root_folder);

    writer.add_scalar_metadata<uint64_t>("pulse_id");
    writer.add_scalar_metadata<uint64_t>("frame_id");
    writer.add_scalar_metadata<uint32_t>("daq_rec");
    writer.add_scalar_metadata<uint16_t>("received_packets");

    writer.set_pulse_id(1);

    writer.write_data(buffer.get());

    writer.write_scalar_metadata(
        "pulse_id",
        &(metadata.pulse_id),
        sizeof(uint64_t));

    writer.write_scalar_metadata(
        "frame_id",
        &(metadata.frame_index),
        sizeof(uint64_t));

    writer.write_scalar_metadata(
        "daq_rec",
        &(metadata.daq_rec),
        sizeof(uint32_t));

    writer.write_scalar_metadata(
        "received_packets",
        &(metadata.n_recv_packets),
        sizeof(uint16_t));


}
