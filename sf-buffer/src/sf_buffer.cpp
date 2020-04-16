#include <iostream>
#include <stdexcept>
#include <RingBuffer.hpp>
#include <UdpRecvModule.hpp>
#include <H5Writer.hpp>
#include <WriterUtils.hpp>

#include "config.hpp"
#include "jungfrau.hpp"
#include "buffer_utils.hpp"


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
    H5Writer writer("");

    uint64_t n_stat_out(0);
    uint64_t n_frames_with_missing_packets = 0;
    uint64_t n_missed_frames = 0;
    uint64_t last_pulse_id = 0;

    // TODO: Ugly hack.
    std::stringstream latest_filename;
    latest_filename << root_folder << "/";
    latest_filename << device_name << "/";
    latest_filename << "LATEST";
    string str_latest_filename = latest_filename.str();

    auto* buffer_pulse_id = new uint64_t[FILE_MOD];
    memset((char*) buffer_pulse_id, 0, FILE_MOD * 8);

    auto* buffer_frame_id = new uint64_t[FILE_MOD];
    memset((char*) buffer_frame_id, 0, FILE_MOD * 8);

    auto* buffer_daq_rec = new uint32_t[FILE_MOD];
    memset((char*) buffer_daq_rec, 0, FILE_MOD * 4);

    auto* buffer_recv_packets_1 = new uint64_t[FILE_MOD];
    memset((char*) buffer_recv_packets_1, 0, FILE_MOD * 8);

    auto* buffer_recv_packets_2 = new uint64_t[FILE_MOD];
    memset((char*) buffer_recv_packets_2, 0, FILE_MOD * 8);

    auto* buffer_recv_packets = new uint16_t[FILE_MOD];
    memset((char*) buffer_recv_packets, 0, FILE_MOD * 2);

    while (true) {
        auto data = ring_buffer.read();

        if (data.first == nullptr) {
            this_thread::sleep_for(chrono::milliseconds(10));
            continue;
        }

        auto pulse_id = data.first->pulse_id;

        auto frame_file = get_filename(
                root_folder,
                device_name,
                pulse_id);

        if (current_file != frame_file) {
            // TODO: This executes only in first loop. Fix it.
            if (writer.is_file_open()) {
                writer.write_data("pulse_id", 0, (char*) buffer_pulse_id,
                                  {1}, 8*FILE_MOD, "uint64", "little");

                writer.write_data("frame_id", 0, (char*) buffer_frame_id,
                                  {1}, 8*FILE_MOD, "uint64", "little");

                writer.write_data("daq_rec", 0, (char*) buffer_daq_rec,
                                  {1}, 4*FILE_MOD, "uint32", "little");

                writer.write_data("recv_packets_1", 0, (char*) buffer_recv_packets_1,
                                  {1}, 8*FILE_MOD, "uint64", "little");

                writer.write_data("recv_packets_2", 0, (char*) buffer_recv_packets_2,
                                  {1}, 8*FILE_MOD, "uint64", "little");

                writer.write_data("received_packets", 0, (char*) buffer_recv_packets,
                                  {1}, 2*FILE_MOD, "uint16", "little");

                // TODO: Ugly hack from above, part 2.
                stringstream latest_command;
                latest_command << "echo " << current_file;
                latest_command << " > " << str_latest_filename;
                auto str_latest_command = latest_command.str();

                system(str_latest_command.c_str());

                writer.close_file();
            }

            current_file = frame_file;

            memset((char*) buffer_pulse_id, 0, FILE_MOD * 8);
            memset((char*) buffer_frame_id, 0, FILE_MOD * 8);
            memset((char*) buffer_daq_rec, 0, FILE_MOD * 4);
            memset((char*) buffer_recv_packets_1, 0, FILE_MOD * 8);
            memset((char*) buffer_recv_packets_2, 0, FILE_MOD * 8);
            memset((char*) buffer_recv_packets, 0, FILE_MOD * 2);

            WriterUtils::create_destination_folder(current_file);
            writer.create_file(current_file);
        }

        auto file_frame_index = get_file_frame_index(pulse_id);

        writer.write_data(
                "image", file_frame_index,
                data.second, {512,1024},
                JUNGFRAU_DATA_BYTES_PER_FRAME, "uint16", "little");

        memcpy((void*) (buffer_pulse_id + file_frame_index),
               (void*) &(data.first->pulse_id), 8);

        memcpy((void*) (buffer_frame_id + file_frame_index),
              (void*) &(data.first->frame_index), 8);

        memcpy((void*) (buffer_daq_rec + file_frame_index),
              (void*) &(data.first->daq_rec), 8);

        memcpy((void*) (buffer_recv_packets_1 + file_frame_index),
              (void*) &(data.first->recv_packets_1), 8);

        memcpy((void*) (buffer_recv_packets_2 + file_frame_index),
              (void*) &(data.first->recv_packets_2), 8);

        memcpy((void*) (buffer_recv_packets + file_frame_index),
               (void*) &(data.first->n_recv_packets), 2);

        ring_buffer.release(data.first->buffer_slot_index);

        // TODO: Make real statistics, please.
        n_stat_out++;

        if (data.first->recv_packets_2 > 0 || data.first->recv_packets_1 > 0) {
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

    delete [] buffer_pulse_id;
    delete [] buffer_frame_id;
    delete [] buffer_daq_rec;
    delete [] buffer_recv_packets_1;
    delete [] buffer_recv_packets_2;
}
