#include "gtest/gtest.h"
#include "H5WriteModule.hpp"

#include <thread>
#include <string>
#include "RingBuffer.hpp"

#include "mock/TestH5Format.cpp"

using namespace std;

void generate_frames(RingBuffer& ring_buffer)
{
    // TODO: FIll the ring_buffer with test data.
}

TEST(H5WriteModule, basic_interaction)
{
    TestH5Format format("start_dataset");
    RingBuffer ring_buffer(10);
    H5WriteModule h5_write_module(ring_buffer, {}, format);
}