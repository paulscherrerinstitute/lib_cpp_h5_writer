#include <BinaryWriter.hpp>
#include "BufferUtils.hpp"
#include <fcntl.h>
#include "gtest/gtest.h"

TEST(BinaryWriter, basic_interaction)
{
    auto root_folder = ".";
    auto device_name = "test_device";
    uint64_t pulse_id = 5;

    BinaryWriter writer(device_name, root_folder);

    JFFileFormat frame_data;
    frame_data.pulse_id = 1;
    frame_data.frame_id = 2;
    frame_data.daq_rec = 3;
    frame_data.n_recv_packets = 4;

    ASSERT_EQ(frame_data.FORMAT_MARKER, JF_FORMAT_START_BYTE);

    writer.write(5, &frame_data);

    auto output_filename =
            BufferUtils::get_filename(root_folder, device_name, pulse_id);

    auto read_fd = open(output_filename.c_str(), O_RDONLY);
    ASSERT_NE(read_fd, -1);

    auto file_frame_index = BufferUtils::get_file_frame_index(pulse_id);

    JFFileFormat read_data;

    ::lseek(read_fd, file_frame_index * sizeof(JFFileFormat), SEEK_SET);
    ::read(read_fd, &read_data, sizeof(JFFileFormat));

    ASSERT_EQ(frame_data.FORMAT_MARKER, JF_FORMAT_START_BYTE);
    ASSERT_EQ(frame_data.FORMAT_MARKER, read_data.FORMAT_MARKER);
    ASSERT_EQ(frame_data.pulse_id, read_data.pulse_id);
    ASSERT_EQ(frame_data.frame_id, read_data.frame_id);
    ASSERT_EQ(frame_data.daq_rec, read_data.daq_rec);
    ASSERT_EQ(frame_data.n_recv_packets, read_data.n_recv_packets);
}

TEST(BinaryWriter, test_format_marker)
{
    auto root_folder = ".";
    auto device_name = "test_device";
    uint64_t pulse_id = 5;

    BinaryWriter writer(device_name, root_folder);

    JFFileFormat frame_data;
    frame_data.pulse_id = 1;
    frame_data.frame_id = 2;
    frame_data.daq_rec = 3;
    frame_data.n_recv_packets = 4;

    writer.write(5, &frame_data);

    auto output_filename =
            BufferUtils::get_filename(root_folder, device_name, pulse_id);

    auto read_fd = open(output_filename.c_str(), O_RDONLY);
    ASSERT_NE(read_fd, -1);

    auto file_frame_index = BufferUtils::get_file_frame_index(pulse_id);

    JFFileFormat read_data;

    // One frame before should be empty.
    ::lseek(read_fd, (file_frame_index-1) * sizeof(JFFileFormat), SEEK_SET);
    ::read(read_fd, &read_data, sizeof(JFFileFormat));
    ASSERT_NE(read_data.FORMAT_MARKER, JF_FORMAT_START_BYTE);

    // One frame after should be empty as well.
    ::lseek(read_fd, (file_frame_index+1) * sizeof(JFFileFormat), SEEK_SET);
    ::read(read_fd, &read_data, sizeof(JFFileFormat));
    ASSERT_NE(read_data.FORMAT_MARKER, JF_FORMAT_START_BYTE);

    // But this frame should be here.
    ::lseek(read_fd, (file_frame_index) * sizeof(JFFileFormat), SEEK_SET);
    ::read(read_fd, &read_data, sizeof(JFFileFormat));
    ASSERT_EQ(read_data.FORMAT_MARKER, JF_FORMAT_START_BYTE);

}