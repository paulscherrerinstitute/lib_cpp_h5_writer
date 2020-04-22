#include <iostream>
#include <stdexcept>
#include "config.hpp"
#include "zmq.h"
#include <string>
#include <RingBuffer.hpp>
#include <BufferUtils.hpp>
#include <jungfrau.hpp>

using namespace std;

int main (int argc, char *argv[])
{
    if (argc != 4) {
        cout << endl;
        cout << "Usage: sf_h5_writer ";
        cout << " [output_file] [start_pulse_id] [stop_pulse_id]";
        cout << endl;
        cout << "\toutput_file: Complete path to the output file." << endl;
        cout << "\tstart_pulse_id: Start pulse_id of retrieval." << endl;
        cout << "\tstop_pulse_id: Stop pulse_id of retrieval." << endl;
        cout << endl;

        exit(-1);
    }

    string output_file = string(argv[1]);
    uint64_t start_pulse_id = (uint64_t) atoll(argv[2]);
    uint64_t stop_pulse_id = (uint64_t) atoll(argv[3]);

    size_t n_modules = 32;

    auto ctx = zmq_ctx_new();
    zmq_ctx_set (ctx, ZMQ_IO_THREADS, 16);

    auto socket = zmq_socket(ctx, ZMQ_PULL);

    //TODO: Use ipc?
    if (zmq_bind(socket, "tcp://127.0.0.1:50000") != 0) {
        throw runtime_error(strerror (errno));
    }

    int rcvhwm = 10000;
    if (zmq_setsockopt(socket, ZMQ_RCVHWM, &rcvhwm, sizeof(rcvhwm)) != 0) {
        throw runtime_error(strerror (errno));
    }

    int linger = 0;
    if (zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger)) != 0) {
        throw runtime_error(strerror (errno));
    }

    auto metadata_buffer = make_unique<ModuleFrame>();

    auto image_buffer = make_unique<uint16_t[]>(512 * 1024);

    while (true) {
        auto n_bytes_metadata = zmq_recv(
                socket,
                metadata_buffer.get(),
                sizeof(ModuleFrame),
                0);

        if (n_bytes_metadata != sizeof(ModuleFrame)) {
            throw runtime_error("Unexpected number of bytes in metadata.");
        }

        auto n_bytes_image = zmq_recv(
                socket,
                image_buffer.get(),
                512 * 1024 * 2,
                0);

        if (n_bytes_image != 512 * 1024 * 2) {
            cout << "n_bytes_image " << n_bytes_image << endl;
            throw runtime_error("Unexpected number of bytes in image.");
        }

        cout << "Received " << metadata_buffer->pulse_id;
        cout << " from " << metadata_buffer->module_id;
        cout << endl;
    }

    zmq_close(socket);
    zmq_ctx_destroy(ctx);

    return 0;
}
