#ifndef MOCK_STREAM_H
#define MOCK_STREAM_H

#include <zmq.hpp>

const std::string MOCK_STREAM_ADDRESS("tcp://127.0.0.1:11000");

void generate_stream(size_t n_messages)
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PUSH);
    socket.bind(MOCK_STREAM_ADDRESS);

    std::string header =
            "{\"frame\": 0, \"shape\": [1,16], \"type\": \"uint8\"}";
    zmq::const_buffer header_msg(header.c_str(), header.length());

    size_t buffer_size = 16;
    char buffer[buffer_size];
    zmq::const_buffer buffer_msg(buffer, buffer_size);

    for (size_t i=0; i<n_messages; i++) {
        socket.send(header_msg, zmq::send_flags::sndmore);
        socket.send(buffer_msg);
    }
}

#endif