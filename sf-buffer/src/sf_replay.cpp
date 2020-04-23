#include <iostream>
#include <UdpReceiver.hpp>
#include <thread>
#include "jungfrau.hpp"
#include "BufferUtils.hpp"
#include "zmq.h"
#include "buffer_config.hpp"

using namespace std;
using namespace core_buffer;

struct FileBufferMetadata {
    uint64_t pulse_id[REPLAY_BLOCK_SIZE];
    uint64_t frame_index[REPLAY_BLOCK_SIZE];
    uint32_t daq_rec[REPLAY_BLOCK_SIZE];
    uint16_t n_received_packets[REPLAY_BLOCK_SIZE];
};

void load_data_from_file (
        FileBufferMetadata* metadata_buffer,
        char* image_buffer,
        const string &filename,
        const size_t start_index)
{

    hsize_t image_dim[3] = {FILE_MOD, 512, 1024};
    H5::DataSpace image_space (3, image_dim);
    hsize_t i_count[] = {REPLAY_BLOCK_SIZE, 512, 1024};
    hsize_t i_start[] = {start_index, 0, 0};
    image_space.selectHyperslab(H5S_SELECT_SET, i_count, i_start);

    hsize_t metadata_dim[2] = {FILE_MOD, 1};
    H5::DataSpace metadata_space (2, metadata_dim);
    hsize_t m_count[] = {REPLAY_BLOCK_SIZE, 1};
    hsize_t m_start[] = {start_index, 0};
    metadata_space.selectHyperslab(H5S_SELECT_SET, m_count, m_start);

    H5::H5File input_file(filename, H5F_ACC_RDONLY);

    auto image_dataset = input_file.openDataSet("image");
    image_dataset.read(
            image_buffer, H5::PredType::NATIVE_UINT16,
            H5::DataSpace::ALL, image_space);

    auto pulse_id_dataset = input_file.openDataSet("pulse_id");
    pulse_id_dataset.read(
            metadata_buffer->pulse_id, H5::PredType::NATIVE_UINT64,
            H5::DataSpace::ALL, metadata_space);

    auto frame_id_dataset = input_file.openDataSet("frame_id");
    frame_id_dataset.read(
            metadata_buffer->frame_index, H5::PredType::NATIVE_UINT64,
            H5::DataSpace::ALL, metadata_space);

    auto daq_rec_dataset = input_file.openDataSet("daq_rec");
    daq_rec_dataset.read(
            metadata_buffer->daq_rec, H5::PredType::NATIVE_UINT32,
            H5::DataSpace::ALL, metadata_space);

    auto received_packets_dataset =
            input_file.openDataSet("received_packets");
    received_packets_dataset.read(
            metadata_buffer->n_received_packets,
            H5::PredType::NATIVE_UINT16,
            H5::DataSpace::ALL, metadata_space);

    input_file.close();
}

int main (int argc, char *argv[]) {

    if (argc != 6) {
        cout << endl;
        cout << "Usage: sf_replay [device]";
        cout << " [channel_name] [module_id] [start_pulse_id] [stop_pulse_id]";
        cout << endl;
        cout << "\tdevice: Name of detector." << endl;
        cout << "\tchannel_name: M00-M31 for JF16M." << endl;
        cout << "\tmodule_id: Module index" << endl;
        cout << "\tstart_pulse_id: Start pulse_id of retrieval." << endl;
        cout << "\tstop_pulse_id: Stop pulse_id of retrieval." << endl;
        cout << endl;

        exit(-1);
    }

    string device = string(argv[1]);
    string channel_name = string(argv[2]);
    uint16_t module_id = (uint16_t) atoi(argv[3]);
    uint64_t start_pulse_id = (uint64_t) atoll(argv[4]);
    uint64_t stop_pulse_id = (uint64_t) atoll(argv[5]);

    auto ctx = zmq_ctx_new();
    auto socket = zmq_socket(ctx, ZMQ_PUSH);

    int sndhwm = REPLAY_BLOCK_SIZE;
    if (zmq_setsockopt(socket, ZMQ_SNDHWM, &sndhwm, sizeof(sndhwm)) != 0) {
        throw runtime_error(strerror (errno));
    };
    int linger_ms = 0;
    if (zmq_setsockopt(socket, ZMQ_LINGER, &linger_ms, sizeof(linger_ms))) {
        throw runtime_error(strerror (errno));
    }
    stringstream ipc_addr;
    ipc_addr << "ipc://sf-replay-" << (int)module_id;
    auto ipc = ipc_addr.str();
    if (zmq_connect(socket, ipc.c_str()) != 0) {
        throw runtime_error(strerror (errno));
    }

    auto metadata_buffer = make_unique<FileBufferMetadata>();
    auto image_buffer = make_unique<uint16_t[]>(
            REPLAY_BLOCK_SIZE * MODULE_N_PIXELS);

    auto path_suffixes =
            BufferUtils::get_path_suffixes(start_pulse_id, stop_pulse_id);

    size_t current_pulse_id = start_pulse_id;

    for (const auto& suffix:path_suffixes) {

        string filename =
                device + "/" +
                channel_name + "/" +
                suffix.path;

        for (
                size_t i_batch=0;
                i_batch < FILE_MOD;
                i_batch + REPLAY_BLOCK_SIZE)
        {
            load_data_from_file(
                    metadata_buffer.get(),
                    (char*)(image_buffer.get()),
                    filename,
                    i_batch);

            for (size_t i_frame=0; i_frame < REPLAY_BLOCK_SIZE; i_frame++) {

                ModuleFrame module_frame = {
                        metadata_buffer->pulse_id[i_frame],
                        metadata_buffer->frame_index[i_frame],
                        metadata_buffer->daq_rec[i_frame],
                        metadata_buffer->n_received_packets[i_frame],
                        module_id
                };

                if (module_frame.pulse_id != current_pulse_id) {
                    cout << "Unexpected pulse_id for module " << module_id;
                    cout << " Got " << module_frame.pulse_id;
                    cout << " expected " << current_pulse_id;
                }

                zmq_send(socket,
                         &module_frame,
                         sizeof(ModuleFrame),
                         ZMQ_SNDMORE);

                auto buff_offset = i_frame * MODULE_N_PIXELS;
                zmq_send(socket,
                         (char*)(image_buffer.get() + buff_offset),
                         MODULE_N_BYTES,
                         0);

                current_pulse_id++;
            }
        }
    }

    zmq_close(socket);
    zmq_ctx_destroy(ctx);
}
