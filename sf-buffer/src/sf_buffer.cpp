#include <iostream>
#include <stdexcept>
#include <RingBuffer.hpp>
#include <UdpRecvModule.hpp>
#include <FastH5Writer.hpp>
#include <UdpReceiver.hpp>

#include "config.hpp"
#include "jungfrau.hpp"
#include "BufferUtils.hpp"

using namespace std;

void write_frame(
        FastH5Writer &writer,
        const uint64_t pulse_id,
        const UdpFrameMetadata* frame_metadata,
        const char* frame_buffer)
{
    writer.set_pulse_id(pulse_id);

    writer.write_data(frame_buffer);

    writer.write_scalar_metadata<uint64_t>(
            "pulse_id", &(frame_metadata->pulse_id));

    writer.write_scalar_metadata<uint64_t>(
            "frame_id",
            &(frame_metadata->frame_index));

    writer.write_scalar_metadata<uint32_t>(
            "daq_rec",
            &(frame_metadata->daq_rec));

    writer.write_scalar_metadata<uint16_t>(
            "received_packets",
            &(frame_metadata->n_recv_packets));
}


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

    uint64_t n_stat_out(0);
    uint64_t n_frames_with_missing_packets = 0;
    uint64_t n_missed_frames = 0;
    uint64_t last_pulse_id = 0;

    FastH5Writer writer(
            BufferUtils::FILE_MOD, 512, 1024, device_name, root_folder);

    writer.add_scalar_metadata<uint64_t>("pulse_id");
    writer.add_scalar_metadata<uint64_t>("frame_id");
    writer.add_scalar_metadata<uint32_t>("daq_rec");
    writer.add_scalar_metadata<uint16_t>("received_packets");

    jungfrau_packet packet_buffer;
    UdpReceiver udp_receiver;
    udp_receiver.bind(udp_port);

    char* previous_frame_buffer = new char[2*512*1024];
    memset(previous_frame_buffer, 0, JUNGFRAU_DATA_BYTES_PER_FRAME);

    UdpFrameMetadata previous_metadata;
    previous_metadata.pulse_id = 0;
    previous_metadata.n_recv_packets = 0;
    previous_metadata.daq_rec = 0;
    previous_metadata.n_recv_packets = 0;

    char* current_frame_buffer = new char[2*512*1024];
    memset(current_frame_buffer, 0, JUNGFRAU_DATA_BYTES_PER_FRAME);

    UdpFrameMetadata current_metadata;
    current_metadata.pulse_id = 0;
    current_metadata.n_recv_packets = 0;
    current_metadata.daq_rec = 0;
    current_metadata.n_recv_packets = 0;

    while (true) {

        while (true) {

            if (!udp_receiver.receive(
                    &packet_buffer,
                    JUNGFRAU_BYTES_PER_PACKET)) {
                continue;
            }

            if (current_metadata.pulse_id != packet_buffer.bunchid) {
                if (current_metadata.pulse_id != 0) {
                    // Commit
                    previous_metadata = current_metadata;
                    swap(previous_frame_buffer, current_frame_buffer);
                }

                // Init current_metadata
                current_metadata.frame_index = packet_buffer.framenum;
                current_metadata.pulse_id = packet_buffer.bunchid;
                current_metadata.daq_rec = packet_buffer.debug;
                current_metadata.n_recv_packets = 0;
                memset(current_frame_buffer, 0, JUNGFRAU_DATA_BYTES_PER_FRAME);
            }

            size_t frame_buffer_offset =
                    JUNGFRAU_DATA_BYTES_PER_PACKET * packet_buffer.packetnum;

            memcpy((void*) (current_frame_buffer + frame_buffer_offset),
                    packet_buffer.data,
                    JUNGFRAU_DATA_BYTES_PER_PACKET);

            current_metadata.n_recv_packets++;

            // Frame finished with last packet.
            if (packet_buffer.packetnum == JUNGFRAU_N_PACKETS_PER_FRAME-1)
            {
                // Commit.
                previous_metadata = current_metadata;
                swap(previous_frame_buffer, current_frame_buffer);

                // This will cause a reset on the next iteration.
                current_metadata.pulse_id = 0;
            }

            if (previous_metadata.pulse_id != 0) {
                break;
            }
        }

        uint64_t pulse_id = previous_metadata.pulse_id;
        write_frame(writer, pulse_id, &previous_metadata, previous_frame_buffer);

        // Indicates that is processed.
        previous_metadata.pulse_id = 0;

        // TODO: Make real statistics, please.
        n_stat_out++;

        if (previous_metadata.n_recv_packets < JUNGFRAU_N_PACKETS_PER_FRAME) {
            n_frames_with_missing_packets +=
                    JUNGFRAU_N_PACKETS_PER_FRAME -
                    previous_metadata.n_recv_packets;
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

        this_thread::yield();
    }
}
