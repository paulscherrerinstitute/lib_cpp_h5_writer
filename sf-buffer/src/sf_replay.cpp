#include <iostream>
#include <UdpReceiver.hpp>
#include <thread>
#include "jungfrau.hpp"
#include "BufferUtils.hpp"
#include "zmq.h"

using namespace std;

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

    // TODO: If stop_pulse_id not in LATEST_filename file path throw exception.
    // string LATEST_filename = root_folder + "/" + device_name + "/LATEST";

    auto metadata_buffer = make_unique<BufferUtils::FileBufferMetadata>();
    metadata_buffer->module_id = module_id;

    auto image_buffer = make_unique<uint16_t[]>(
            BufferUtils::STREAM_BLOCK_SIZE * 512 * 1024);

    auto path_suffixes = BufferUtils::get_path_suffixes(
            start_pulse_id, stop_pulse_id);

    auto ctx = zmq_ctx_new();

    auto socket = zmq_socket(ctx, ZMQ_PUSH);
    int sndhwm = 1;
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

    for (const auto& suffix:path_suffixes) {
        metadata_buffer->start_pulse_id = suffix.start_pulse_id;
        metadata_buffer->stop_pulse_id = suffix.stop_pulse_id;

        string filename =
                device + "/" +
                channel_name + "/" +
                suffix.path;

        cout << "Reading file " << filename << endl;

        H5::H5File input_file(filename, H5F_ACC_RDONLY);

        auto image_dataset = input_file.openDataSet("image");
        auto pulse_id_dataset = input_file.openDataSet("pulse_id");
        auto frame_id_dataset = input_file.openDataSet("frame_id");
        auto daq_rec_dataset = input_file.openDataSet("daq_rec");
        auto received_packets_dataset =
                input_file.openDataSet("received_packets");

        // Load first

        hsize_t file_dim[3] = {BufferUtils::FILE_MOD, 512, 1024};
        H5::DataSpace file_space (3, file_dim);
        hsize_t b_count[] = {BufferUtils::STREAM_BLOCK_SIZE, 512, 1024};
        hsize_t b_start[] = {0, 0, 0};
        file_space.selectHyperslab(H5S_SELECT_SET, b_count, b_start);

        image_dataset.read(
                image_buffer.get(), H5::PredType::NATIVE_UINT16,
                H5::DataSpace::ALL, file_space);

        hsize_t meta_dim[2] = {BufferUtils::FILE_MOD, 1};
        H5::DataSpace meta_space (2, meta_dim);
        hsize_t m_count[] = {BufferUtils::STREAM_BLOCK_SIZE, 1};
        hsize_t m_start[] = {0, 0, 0};
        meta_space.selectHyperslab(H5S_SELECT_SET, m_count, m_start);

        pulse_id_dataset.read(
                metadata_buffer->pulse_id, H5::PredType::NATIVE_UINT64,
                H5::DataSpace::ALL, meta_space);

        frame_id_dataset.read(
                metadata_buffer->frame_index, H5::PredType::NATIVE_UINT64,
                H5::DataSpace::ALL, meta_space);

        daq_rec_dataset.read(
                metadata_buffer->daq_rec, H5::PredType::NATIVE_UINT32,
                H5::DataSpace::ALL, meta_space);

        received_packets_dataset.read(
                metadata_buffer->n_received_packets,
                H5::PredType::NATIVE_UINT16,
                H5::DataSpace::ALL, meta_space);

        for (
                size_t start_pulse=0;
                start_pulse<BufferUtils::FILE_MOD;
                start_pulse+BufferUtils::STREAM_BLOCK_SIZE) {

                for (size_t i_frame=0; i_frame < BufferUtils::STREAM_BLOCK_SIZE; i_frame++) {
                    ModuleFrame module_frame = {
                            metadata_buffer->pulse_id[i_frame],
                            metadata_buffer->frame_index[i_frame],
                            metadata_buffer->daq_rec[i_frame],
                            metadata_buffer->n_received_packets[i_frame],
                            module_id
                    };

                    zmq_send(socket,
                             &module_frame,
                             sizeof(ModuleFrame),
                             ZMQ_SNDMORE);

                    zmq_send(socket,
                             (char*) (image_buffer.get() + (i_frame * 512 * 1024)),
                             512 * 1024 * 2,
                             0);
                }

                // Load next
                if (start_pulse + BufferUtils::STREAM_BLOCK_SIZE < BufferUtils::FILE_MOD) {

                    hsize_t file_dim[3] = {BufferUtils::FILE_MOD, 512, 1024};
                    H5::DataSpace file_space (3, file_dim);
                    hsize_t b_count[] = {BufferUtils::STREAM_BLOCK_SIZE, 512, 1024};
                    hsize_t b_start[] = {start_pulse, 0, 0};
                    file_space.selectHyperslab(H5S_SELECT_SET, b_count, b_start);

                    image_dataset.read(
                            image_buffer.get(), H5::PredType::NATIVE_UINT16,
                            H5::DataSpace::ALL, file_space);

                    hsize_t meta_dim[2] = {BufferUtils::FILE_MOD, 1};
                    H5::DataSpace meta_space (2, meta_dim);
                    hsize_t m_count[] = {BufferUtils::STREAM_BLOCK_SIZE, 1};
                    hsize_t m_start[] = {start_pulse, 0, 0};
                    meta_space.selectHyperslab(H5S_SELECT_SET, m_count, m_start);

                    pulse_id_dataset.read(
                            metadata_buffer->pulse_id, H5::PredType::NATIVE_UINT64,
                            H5::DataSpace::ALL, meta_space);

                    frame_id_dataset.read(
                            metadata_buffer->frame_index, H5::PredType::NATIVE_UINT64,
                            H5::DataSpace::ALL, meta_space);

                    daq_rec_dataset.read(
                            metadata_buffer->daq_rec, H5::PredType::NATIVE_UINT32,
                            H5::DataSpace::ALL, meta_space);

                    received_packets_dataset.read(
                            metadata_buffer->n_received_packets,
                            H5::PredType::NATIVE_UINT16,
                            H5::DataSpace::ALL, meta_space);
                }
            }

        input_file.close();

    }

    zmq_close(socket);
    zmq_ctx_destroy(ctx);
}
