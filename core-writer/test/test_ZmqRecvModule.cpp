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

    zmq_recv_module.start_writing();
    zmq_recv_module.stop_writing();

    zmq_recv_module.stop_recv();
}

TEST(ZmqRecvModule, simple_recv)
{
    size_t n_msg = 1;

    thread sender(generate_stream, n_msg);
    RingBuffer ring_buffer(n_msg);

    ZmqRecvModule zmq_recv_module(ring_buffer, {});
    zmq_recv_module.start_writing();
    zmq_recv_module.start_recv("tcp://127.0.0.1:11000", 4);

    sender.join();
    this_thread::sleep_for(chrono::milliseconds(100));

    zmq_recv_module.stop_recv();

    for (size_t i=0;i<n_msg;i++) {
        auto data = ring_buffer.read();
        // nullptr means there is no data in the buffer.
        ASSERT_TRUE(data.first != nullptr);
        ASSERT_TRUE(data.second != nullptr);
    }

    // no more messages in the buffer.
    auto data = ring_buffer.read();
    ASSERT_TRUE(data.first == nullptr);

    ASSERT_TRUE(ring_buffer.is_empty());
}
