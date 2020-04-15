#include <netinet/in.h>
#include <jungfrau.hpp>
#include "gtest/gtest.h"
#include "UdpReceiver.hpp"
#include "mock/udp.hpp"


TEST(UdpReceiver, simple_recv)
{
    uint16_t udp_port = 12000;

    auto send_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
    ASSERT_TRUE(send_socket_fd >= 0);

    UdpReceiver udp_receiver;
    udp_receiver.bind(udp_port);

    jungfrau_packet send_udp_buffer;
    send_udp_buffer.packetnum = 91;
    send_udp_buffer.framenum = 92;
    send_udp_buffer.bunchid = 93;
    send_udp_buffer.debug = 94;

    auto server_address = get_server_address(udp_port);
    ::sendto(
            send_socket_fd,
            &send_udp_buffer,
            JUNGFRAU_BYTES_PER_PACKET,
            0,
            (sockaddr*) &server_address,
            sizeof(server_address));

    jungfrau_packet recv_udp_buffer;
    ASSERT_TRUE(udp_receiver.receive(
            &recv_udp_buffer, JUNGFRAU_BYTES_PER_PACKET));

    EXPECT_EQ(send_udp_buffer.packetnum, recv_udp_buffer.packetnum);
    EXPECT_EQ(send_udp_buffer.framenum, recv_udp_buffer.framenum);
    EXPECT_EQ(send_udp_buffer.bunchid, recv_udp_buffer.bunchid);
    EXPECT_EQ(send_udp_buffer.debug, recv_udp_buffer.debug);

    ASSERT_FALSE(udp_receiver.receive(
            &recv_udp_buffer, JUNGFRAU_BYTES_PER_PACKET));

    udp_receiver.close();
    ::close(send_socket_fd);
}

TEST(UdpReceiver, false_recv)
{
    uint16_t udp_port = 12000;

    auto send_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
    ASSERT_TRUE(send_socket_fd >= 0);

    UdpReceiver udp_receiver;
    udp_receiver.bind(udp_port);

    jungfrau_packet send_udp_buffer;
    jungfrau_packet recv_udp_buffer;

    auto server_address = get_server_address(udp_port);

    ::sendto(
            send_socket_fd,
            &send_udp_buffer,
            JUNGFRAU_BYTES_PER_PACKET-1,
            0,
            (sockaddr*) &server_address,
            sizeof(server_address));

    ASSERT_FALSE(udp_receiver.receive(
            &recv_udp_buffer, JUNGFRAU_BYTES_PER_PACKET));

    ::sendto(
            send_socket_fd,
            &send_udp_buffer,
            JUNGFRAU_BYTES_PER_PACKET,
            0,
            (sockaddr*) &server_address,
            sizeof(server_address));

    ASSERT_TRUE(udp_receiver.receive(
            &recv_udp_buffer, JUNGFRAU_BYTES_PER_PACKET));

    ::sendto(
            send_socket_fd,
            &send_udp_buffer,
            JUNGFRAU_BYTES_PER_PACKET-1,
            0,
            (sockaddr*) &server_address,
            sizeof(server_address));

    ASSERT_TRUE(udp_receiver.receive(
            &recv_udp_buffer, JUNGFRAU_BYTES_PER_PACKET-1));

    udp_receiver.close();
    ::close(send_socket_fd);
}