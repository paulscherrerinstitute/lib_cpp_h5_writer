#include "SFWriter.hpp"
#include "gtest/gtest.h"

using namespace core_buffer;

TEST(SFWriter, basic_interaction)
{
    size_t n_modules = 2;
    size_t n_frames = 5;

    RingBuffer<DetectorFrame> ring_buffer(10);
    ring_buffer.initialize(MODULE_N_BYTES*n_modules);

    auto data = make_unique<char[]>(n_modules*MODULE_N_BYTES);
    auto metadata = make_shared<DetectorFrame>();
    metadata->frame_bytes_size = n_modules*MODULE_N_BYTES;

    auto* rb_pointer = ring_buffer.reserve(metadata);
    memcpy(rb_pointer, data.get(), n_modules*MODULE_N_BYTES);
    ring_buffer.commit(metadata);

    auto rb_data = ring_buffer.read();

    SFWriter writer("ignore.h5", n_frames, n_modules);
    writer.write(rb_data.first, rb_data.second);
    writer.close_file();

    // TODO: Write some test.
}