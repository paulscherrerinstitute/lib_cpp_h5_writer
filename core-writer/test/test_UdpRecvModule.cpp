#include "gtest/gtest.h"
#include "UdpRecvModule.hpp"
#include "jungfrau.hpp"
#include "mock/udp.hpp"

using namespace std;

TEST(UdpRecvModule, basic_interaction)
{
    uint16_t udp_port(12000);

    RingBuffer<UdpFrameMetadata> ring_buffer(10);
    UdpRecvModule udp_recv_module(ring_buffer);

    udp_recv_module.start_recv(udp_port, JUNGFRAU_BYTES_PER_PACKET);

    EXPECT_THROW(
            udp_recv_module.start_recv(udp_port, JUNGFRAU_BYTES_PER_PACKET),
            runtime_error);

    // Stop should never throw an exception.
    udp_recv_module.stop_recv();
    EXPECT_NO_THROW(udp_recv_module.stop_recv());
}

TEST(UdpRecvModule, simple_recv)
{
    uint16_t udp_port(MOCK_UDP_PORT);
    size_t n_msg(128);

    RingBuffer<UdpFrameMetadata> ring_buffer(10);
    UdpRecvModule udp_recv_module(ring_buffer);

    udp_recv_module.start_recv(udp_port, JUNGFRAU_BYTES_PER_PACKET);

    this_thread::sleep_for(chrono::milliseconds(100));

    ASSERT_TRUE(ring_buffer.is_empty());

    auto send_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
    ASSERT_TRUE(send_socket_fd >= 0);

    auto server_address = get_server_address(udp_port);

    jungfrau_packet send_udp_buffer;
    send_udp_buffer.framenum = 1;
    send_udp_buffer.bunchid = 100;
    send_udp_buffer.debug = 1000;

    for (size_t i=0; i<n_msg; i++) {
        send_udp_buffer.packetnum = i;

        ::sendto(
                send_socket_fd,
                &send_udp_buffer,
                JUNGFRAU_BYTES_PER_PACKET,
                0,
                (sockaddr*) &server_address,
                sizeof(server_address));
    }

    ASSERT_TRUE(ring_buffer.is_empty());

    // When packet from new frame is received, the previous frame should be
    // committed to the ring buffer.
    send_udp_buffer.framenum = 2;
    ::sendto(
            send_socket_fd,
            &send_udp_buffer,
            JUNGFRAU_BYTES_PER_PACKET,
            0,
            (sockaddr*) &server_address,
            sizeof(server_address));

    ASSERT_FALSE(ring_buffer.is_empty());

    ::close(send_socket_fd);
}