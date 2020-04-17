#include <iostream>
#include <stdexcept>
#include <RingBuffer.hpp>
#include <UdpRecvModule.hpp>
#include <H5Writer.hpp>
#include <WriterUtils.hpp>
#include "BinaryWriter.hpp"
#include "config.hpp"
#include "jungfrau.hpp"
#include "BufferUtils.hpp"


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

    RingBuffer<UdpFrameMetadata> ring_buffer(config::ring_buffer_n_slots);

    UdpRecvModule udp_module(ring_buffer);
    udp_module.start_recv(udp_port, JUNGFRAU_DATA_BYTES_PER_FRAME);

    string current_file("");

    uint64_t n_stat_out(0);
    uint64_t n_frames_with_missing_packets = 0;
    uint64_t n_missed_frames = 0;
    uint64_t last_pulse_id = 0;

    BinaryWriter writer(device_name, root_folder);
    // TODO: Optimize this away and pass it with the RB.
    auto* jf_file_format_buffer = new JFFileFormat();

    // Wait for 1 event to accumulate.
    this_thread::sleep_for(chrono::milliseconds(10));

    while (true) {
        auto data = ring_buffer.read();

        if (data.first == nullptr) {
            // TODO: Try to sleep at the end of a succesful loop.
            this_thread::sleep_for(chrono::milliseconds(10));
            continue;
        }

        auto* metadata = data.first.get();
        auto pulse_id = metadata->pulse_id;

        jf_file_format_buffer->pulse_id = pulse_id;
        jf_file_format_buffer->frame_id = metadata->frame_index;
        jf_file_format_buffer->daq_rec = metadata->daq_rec;
        jf_file_format_buffer->n_recv_packets = metadata->n_recv_packets;

        memcpy(&(jf_file_format_buffer->data),
                data.second,
                JUNGFRAU_DATA_BYTES_PER_FRAME);

        writer.write(pulse_id, jf_file_format_buffer);

        ring_buffer.release(data.first->buffer_slot_index);

        // TODO: Make real statistics, please.
        n_stat_out++;

        if (data.first->n_recv_packets < JUNGFRAU_N_PACKETS_PER_FRAME) {
            n_frames_with_missing_packets++;
        }

        if (last_pulse_id>0) {
            n_missed_frames += (pulse_id - last_pulse_id) - 1;
        }
        last_pulse_id = pulse_id;

        if (n_stat_out == 500) {
            cout << "device_name " << device_name;
            cout << " pulse_id " << pulse_id;
            cout << " n_missed_frames " << n_missed_frames;
            cout << " f_with_miss_p " << n_frames_with_missing_packets;
            cout << endl;


            n_stat_out = 0;
            n_frames_with_missing_packets = 0;
            n_missed_frames = 0;
        }
    }

    delete jf_file_format_buffer;
}
