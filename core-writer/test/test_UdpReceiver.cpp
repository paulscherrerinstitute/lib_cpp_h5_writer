#include <netinet/in.h>
#include <jungfrau.hpp>
#include "gtest/gtest.h"
#include "UdpReceiver.hpp"

sockaddr_in get_server_address(uint16_t udp_port)
{
    sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(udp_port);

    return server_address;
}

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
    EXPECT_TRUE(udp_receiver.receive(
            &recv_udp_buffer, JUNGFRAU_BYTES_PER_PACKET));

    EXPECT_FALSE(udp_receiver.receive(
            &recv_udp_buffer, JUNGFRAU_BYTES_PER_PACKET));

    udp_receiver.close();
    ::close(send_socket_fd);
}