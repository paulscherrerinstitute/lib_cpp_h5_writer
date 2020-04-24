#include "gtest/gtest.h"
#include "../src/sf_replay.cpp"

using namespace std;


TEST(sf_replay, non_aligned_start)
{
    auto ctx = zmq_ctx_new();
    auto send_socket = zmq_socket(ctx, ZMQ_PUSH);
    auto recv_socket = zmq_socket(ctx, ZMQ_PUSH);

    const int linger_ms = 0;
    zmq_setsockopt(send_socket, ZMQ_LINGER, &linger_ms, sizeof(linger_ms));
    zmq_setsockopt(recv_socket, ZMQ_LINGER, &linger_ms, sizeof(linger_ms));

//    sf_replay();
}