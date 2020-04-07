#include "gtest/gtest.h"
#include "H5WriteModule.hpp"

#include <string>
#include "RingBuffer.hpp"

#include "mock/TestH5Format.cpp"

using namespace std;

// Shape * 2 bytes/value (uint16_t)
size_t image_n_bytes = 1024*2*2;

void generate_frames(RingBuffer& ring_buffer, int n_frames)
{
    size_t y_length = 2;
    size_t x_length = 1024;

    for (int i_frame=0; i_frame < n_frames; i_frame++) {

        FrameMetadata metadata = {
                0, // size_t buffer_slot_index;
                image_n_bytes, // size_t frame_bytes_size;
                static_cast<uint64_t>(i_frame), // uint64_t frame_index;
                "little", // std::string endianness;
                "uint16", //std::string type;
                {y_length, x_length} // std::vector<size_t> frame_shape;
        };

        auto ptr_metadata = make_shared<FrameMetadata>(metadata);
        auto ptr_buffer = ring_buffer.reserve(ptr_metadata);

        auto value_ptr = (uint16_t*) ptr_buffer;

        for (size_t y=0; y<y_length; y++) {
            for (size_t x=0; x<x_length; x++) {
                size_t offset = y*x_length + x;
                value_ptr[offset] = static_cast<uint16_t>(i_frame);
            }
        }

        ring_buffer.commit(ptr_metadata);
    }
}

TEST(H5WriteModule, basic_interaction)
{
    TestH5Format format("start_dataset");

    RingBuffer ring_buffer(10);
    ring_buffer.initialize(image_n_bytes);

    H5WriteModule h5_write_module(ring_buffer, {}, format);

    h5_write_module.start_writing("ignore_out.h5", 5);
    generate_frames(ring_buffer, 5);

    this_thread::sleep_for(chrono::milliseconds(100));

    // Stop should never throw an exception.
    h5_write_module.stop_writing();
    EXPECT_NO_THROW(h5_write_module.stop_writing());

    EXPECT_TRUE(ring_buffer.is_empty());
}