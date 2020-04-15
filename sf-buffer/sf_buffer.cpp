#include <iostream>
#include <stdexcept>
#include <RingBuffer.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <UdpReceiver.hpp>
#include <UdpRecvModule.hpp>
#include <H5Writer.hpp>

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

    RingBuffer<UdpFrameMetadata> ring_buffer(config::ring_buffer_n_slots);

    UdpRecvModule udp_module(ring_buffer);
    udp_module.start_recv(udp_port, JUNGFRAU_DATA_BYTES_PER_FRAME);

    H5Writer writer("output.h5");
    writer.create_file();

    while (true) {
        auto data = ring_buffer.read();

        if (data.first == nullptr) {
            this_thread::sleep_for(chrono::milliseconds(10));
            continue;
        }

        auto pulse_id = data.first->pulse_id;
        // TODO: Make this modulo of pulse_id.
        auto file_frame_index = 0;

        writer.write_data(
                "pulse_id",
                file_frame_index,
                (char*)(&data.first->pulse_id),
                {1},
                8,
                "uint64",
                "little");

        writer.write_data(
                "frame_id",
                file_frame_index,
                (char*)(&data.first->frame_index),
                {1},
                8,
                "uint64",
                "little");

        writer.write_data(
                "daq_rec",
                file_frame_index,
                (char*)(&data.first->daq_rec),
                {1},
                8,
                "uint32",
                "little");

        writer.write_data(
                "recv_packets_1",
                file_frame_index,
                (char*)(&data.first->recv_packets_1),
                {1},
                8,
                "uint64",
                "little");

        writer.write_data(
                "recv_packets_2",
                file_frame_index,
                (char*)(&data.first->recv_packets_2),
                {1},
                8,
                "uint64",
                "little");

    }
}
