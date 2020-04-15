#include <iostream>
#include <stdexcept>
#include <RingBuffer.hpp>
#include <sys/socket.h>
#include <netinet/in.h>

#include "config.hpp"
#include "jungfrau.hpp"


using namespace std;

int main (int argc, char *argv[]) {
    if (argc != 6) {
        cout << endl;
        cout << "Usage: sf_buffer [device_name] [udp_port] [root_folder]";
        cout << endl;
        cout << "\tdevice_name: Name to write to disk.";
        cout << "\tudp_port: UDP port to connect to." << endl;
        cout << "\troot_folder: FS root folder." << endl;
        cout << endl;

        exit(-1);
    }

    string device_name = string(argv[1]);
    int udp_port = atoi(argv[2]);
    string root_folder = string(argv[3]);

    auto socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        throw runtime_error("Cannot open socket.");
    }

    sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(udp_port);

    if(bind(socket_fd,
            reinterpret_cast<const sockaddr *>(&server_address),
            sizeof(server_address)) < 0) {
        throw runtime_error("Cannot bind socket.");
    }

    struct timeval udp_socket_timeout;
    udp_socket_timeout.tv_sec = 0;
    udp_socket_timeout.tv_usec = 100;

    setsockopt(
            socket_fd,
            SOL_SOCKET,
            SO_RCVTIMEO,
            (const char*)&udp_socket_timeout,
            sizeof(struct timeval));

    RingBuffer ring_buffer(config::ring_buffer_n_slots);
    jungfrau_packet packet;

    uint64_t last_framenum = -1;

    while (true) {
        recv(socket_fd, &packet, JUNGFRAU_BYTES_PER_PACKET, 0);

    }
}
