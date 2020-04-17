#include <iostream>
#include <chrono>
#include <UdpReceiver.hpp>
#include "jungfrau.hpp"


using namespace std;

int main (int argc, char *argv[]) {
    if (argc != 4) {
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

    UdpReceiver receiver;
    receiver.bind(udp_port);

    jungfrau_packet recv_buffer;

    uint64_t current_pulse_id = 0;
    uint64_t n_recv_packets = 0;
    uint64_t n_missing_packets = 0;
    uint16_t loop_counter = 0;

    while (true) {
        if (receiver.receive(&recv_buffer, sizeof(jungfrau_packet))) {

            loop_counter++;

            if (recv_buffer.bunchid != current_pulse_id) {
                if (current_pulse_id != 0) {
                    n_missing_packets += (128-n_recv_packets);
                }
                n_recv_packets = 0;
                current_pulse_id = recv_buffer.bunchid;
            }

            n_recv_packets++;

            if (loop_counter == 1000) {

                using namespace date;
                using namespace chrono;

                cout << system_clock::now();
                cout << " miss " << n_missing_packets;
                cout << endl;

                loop_counter = 0;
            }
        }
    }
}
