#include "gtest/gtest.h"
#include "ProcessManager.hpp"

#include "mock/stream.hpp"


TEST(ProcessManager, basic_interaction)
{
    TestH5Format format("start_dataset");
    RingBuffer ring_buffer(10);

    ZmqRecvModule recv_module(ring_buffer, {});
    H5WriteModule write_module(ring_buffer, {}, format);

    ProcessManager manager(write_module, recv_module);

    string output_file("ignore_out.h5");
    int n_frames = 5;
    int user_id = -1;
    size_t n_msg = 10;

    EXPECT_THROW(
            manager.start_writing(output_file, n_frames, user_id),
            runtime_error);

    thread sender(generate_stream, n_msg);
    manager.start_receiving(MOCK_STREAM_ADDRESS, 3);

    this_thread::sleep_for(chrono::milliseconds(100));
    sender.join();

    // Start with an empty RB.
    EXPECT_TRUE(ring_buffer.is_empty());

    // Write first 5 images you receive.
    manager.start_writing(output_file, 5, user_id);
    // Send 6 images, so 1 will be left in the RB.
    thread sender2(generate_stream, 6);
    this_thread::sleep_for(chrono::milliseconds(100));
    sender2.join();

    // Writer stopped because it received all frames.
    EXPECT_FALSE(write_module.is_writing());
    // But there should be one more frame in the RB.
    EXPECT_FALSE(ring_buffer.is_empty());

    // When restarting the writing, RB should be cleared first.
    manager.start_writing(output_file, 5, user_id);
    // Send exact number of needed frames.
    thread sender3(generate_stream, 5);
    this_thread::sleep_for(chrono::milliseconds(100));
    sender3.join();

    // Writer should complete.
    EXPECT_FALSE(write_module.is_writing());
    // There should be no frames left in the RB.
    EXPECT_TRUE(ring_buffer.is_empty());
}
