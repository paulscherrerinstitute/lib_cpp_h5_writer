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

    RingBuffer<UdpFrameMetadata> ring_buffer(config::ring_buffer_n_slots);
    ring_buffer.initialize(JUNGFRAU_DATA_BYTES_PER_FRAME);

    jungfrau_packet packet;

    shared_ptr<UdpFrameMetadata> metadata = nullptr;
    char* frame_buffer = nullptr;

    while (true) {
        auto data_len = recv(socket_fd, &packet, JUNGFRAU_BYTES_PER_PACKET, 0);

        if (data_len < 0) {
            continue;
        }

        if (data_len != JUNGFRAU_BYTES_PER_PACKET) {
            cout << "Invalid packet length " << data_len << endl;
            continue;
        }

        auto* current_metadata = metadata.get();

        if (packet.framenum != current_metadata->frame_index) {
            if (frame_buffer != nullptr) {
                ring_buffer.commit(metadata);
            }

            metadata = make_shared<UdpFrameMetadata>();
            current_metadata = metadata.get();

            current_metadata->frame_index = packet.framenum;
            current_metadata->pulse_id = packet.bunchid;
            current_metadata->frame_bytes_size = JUNGFRAU_DATA_BYTES_PER_FRAME;
            current_metadata->recv_packets_1 = 0;
            current_metadata->recv_packets_2 = 0;

            frame_buffer = ring_buffer.reserve(metadata);
            memset(frame_buffer, 0, JUNGFRAU_DATA_BYTES_PER_FRAME);
        }

        size_t frame_buffer_offset =
                JUNGFRAU_DATA_BYTES_PER_PACKET * packet.packetnum;

        memcpy(
                (void *)frame_buffer[frame_buffer_offset],
                packet.data,
                JUNGFRAU_DATA_BYTES_PER_PACKET);

        if (packet.packetnum < 64) {
            current_metadata->recv_packets_1 ^=
                    (uint64_t)1 << packet.packetnum;
        } else {
            current_metadata->recv_packets_2 ^=
                    (uint64_t)1 << (packet.packetnum - 64);
        }

    }
}
