#include "gtest/gtest.h"
#include "ZmqRecvModule.hpp"

#include <thread>
#include <string>
#include "RingBuffer.hpp"

using namespace std;

void generate_stream(size_t n_messages)
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PUSH);
    socket.bind("tcp://127.0.0.1:11000");

    string header = "{\"frame\": 0, \"shape\": [1,16], \"type\": \"uint8\"}";
    zmq::const_buffer header_msg(header.c_str(), header.length());

    size_t buffer_size = 16;
    char buffer[buffer_size];
    zmq::const_buffer buffer_msg(buffer, buffer_size);

    for (size_t i=0; i<n_messages; i++) {
        socket.send(header_msg, zmq::send_flags::sndmore);
        socket.send(buffer_msg);
    }
}

TEST(ZmqRecvModule, basic_interaction)
{
    RingBuffer ring_buffer(10);
    ZmqRecvModule zmq_recv_module(ring_buffer, {});

    uint8_t n_receivers = 4;
    zmq_recv_module.start_recv("tcp://127.0.0.1:11000", n_receivers);
    EXPECT_THROW(
            zmq_recv_module.start_recv("tcp://127.0.0.1:11000", n_receivers),
            runtime_error);

    zmq_recv_module.start_saving();
    EXPECT_NO_THROW(zmq_recv_module.start_saving());

    // Stop should never throw an exception.
    zmq_recv_module.stop_recv();
    EXPECT_NO_THROW(zmq_recv_module.stop_recv());

    zmq_recv_module.stop_saving_and_clear_buffer();
    EXPECT_NO_THROW(zmq_recv_module.stop_saving_and_clear_buffer());
}

TEST(ZmqRecvModule, simple_recv)
{
    size_t n_msg = 10;

    thread sender(generate_stream, n_msg);
    RingBuffer ring_buffer(n_msg);

    ZmqRecvModule zmq_recv_module(ring_buffer, {});
    zmq_recv_module.start_saving();
    zmq_recv_module.start_recv("tcp://127.0.0.1:11000", 4);

    sender.join();
    this_thread::sleep_for(chrono::milliseconds(100));

    zmq_recv_module.stop_recv();

    for (size_t i=0;i<n_msg;i++) {
        auto data = ring_buffer.read();
        // nullptr means there is no data in the buffer.
        ASSERT_TRUE(data.first != nullptr);
        ASSERT_TRUE(data.second != nullptr);

        ring_buffer.release(data.first->buffer_slot_index);
    }

    // no more messages in the buffer.
    auto data = ring_buffer.read();
    ASSERT_TRUE(data.first == nullptr);

    ASSERT_TRUE(ring_buffer.is_empty());
}

TEST(ZmqRecvModule, stop_saving_and_clear_buffer)
{
    size_t n_msg = 10;

    thread sender(generate_stream, n_msg);
    RingBuffer ring_buffer(n_msg);

    ZmqRecvModule zmq_recv_module(ring_buffer, {});
    zmq_recv_module.start_saving();
    zmq_recv_module.start_recv("tcp://127.0.0.1:11000", 4);

    sender.join();
    this_thread::sleep_for(chrono::milliseconds(100));

    ASSERT_FALSE(ring_buffer.is_empty());
    zmq_recv_module.stop_saving_and_clear_buffer();
    ASSERT_TRUE(ring_buffer.is_empty());

    thread sender2(generate_stream, 2);
    sender2.join();
    this_thread::sleep_for(chrono::milliseconds(100));

    // No messages should be saved from this run.
    ASSERT_TRUE(ring_buffer.is_empty());

    zmq_recv_module.start_saving();

    thread sender3(generate_stream, 2);
    sender3.join();
    this_thread::sleep_for(chrono::milliseconds(100));

    ASSERT_FALSE(ring_buffer.is_empty());

    // stop_recv does not invalidate the buffer.
    zmq_recv_module.stop_recv();
    ASSERT_FALSE(ring_buffer.is_empty());

    zmq_recv_module.stop_saving_and_clear_buffer();
    ASSERT_TRUE(ring_buffer.is_empty());
}
