#include <iostream>
#include <chrono>
#include <UdpReceiver.hpp>
#include <fstream>
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

    auto metadata_buffer = make_unique<FileBufferMetadata>();
    metadata_buffer->module_id = module_id;

    auto image_buffer = make_unique<uint16_t[]>(
            BufferUtils::FILE_MOD * 512 * 1024);

    auto path_suffixes = BufferUtils::get_path_suffixes(
            start_pulse_id, stop_pulse_id);

    auto ctx = zmq_ctx_new();
    auto socket = zmq_socket(ctx, ZMQ_PUSH);
    auto meta_socket = zmq_socket(ctx, ZMQ_SUB);

    int status = 0;

    int sndhwm = 1;
    status += zmq_setsockopt(socket, ZMQ_SNDHWM, &sndhwm, sizeof(sndhwm));
    int linger_ms = 0;
    status += zmq_setsockopt(socket, ZMQ_LINGER, &linger_ms, sizeof(linger_ms));

    //status += zmq_setsockopt(socket, ZMQ_SNDTIMEO, 1000);

    if (status != 0) {
        throw runtime_error(strerror (errno));
    }

    //TODO: Use ipc?
    if (zmq_connect(socket, "ipc://writer") != 0) {
        throw runtime_error(strerror (errno));
    }

    //TODO: Use ipc?
    if (zmq_connect(socket, "ipc://writer_metad") != 0) {
        throw runtime_error(strerror (errno));
    }

    cout << "receiving " << endl;
    uint64_t response;
    zmq_recv(meta_socket, &response, sizeof(response), 0);
    cout << "Done!! " << response << endl;

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
        image_dataset.read(
                image_buffer.get(), H5::PredType::NATIVE_UINT16);

        auto pulse_id_dataset = input_file.openDataSet("pulse_id");
        pulse_id_dataset.read(
                metadata_buffer->pulse_id, H5::PredType::NATIVE_UINT64);

        auto frame_id_dataset = input_file.openDataSet("frame_id");
        frame_id_dataset.read(
                metadata_buffer->frame_index, H5::PredType::NATIVE_UINT64);

        auto daq_rec_dataset = input_file.openDataSet("daq_rec");
        daq_rec_dataset.read(
                metadata_buffer->daq_rec, H5::PredType::NATIVE_UINT32);

        auto received_packets_dataset =
                input_file.openDataSet("received_packets");
        received_packets_dataset.read(
                metadata_buffer->n_received_packets,
                H5::PredType::NATIVE_UINT16);

        input_file.close();

        for (size_t i_frame=0; i_frame<BufferUtils::FILE_MOD; i_frame++) {
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
    }

    zmq_close(socket);
    zmq_ctx_destroy(ctx);
}
