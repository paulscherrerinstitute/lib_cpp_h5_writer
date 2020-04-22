#include <iostream>
#include <stdexcept>
#include "config.hpp"
#include "zmq.h"
#include <string>
#include <RingBuffer.hpp>
#include <BufferUtils.hpp>
#include <jungfrau.hpp>
#include <unordered_map>
#include <thread>
#include <sstream>
#include <chrono>
#include <H5Writer.hpp>

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

    H5Writer writer(output_file);
    writer.create_file();

    auto ctx = zmq_ctx_new();
    zmq_ctx_set (ctx, ZMQ_IO_THREADS, 16);

    size_t n_modules = 32;
    void* sockets[n_modules];

    for (size_t i=0; i<n_modules; i++) {
        sockets[i] = zmq_socket(ctx, ZMQ_PULL);
        int rcvhwm = 100;
        if (zmq_setsockopt(sockets[i], ZMQ_RCVHWM, &rcvhwm, sizeof(rcvhwm)) != 0) {
            throw runtime_error(strerror (errno));
        }
        int linger = 0;
        if (zmq_setsockopt(sockets[i], ZMQ_LINGER, &linger, sizeof(linger)) != 0) {
            throw runtime_error(strerror (errno));
        }

        stringstream ipc_addr;
        ipc_addr << "ipc://sf-replay-" << i;
        auto ipc = ipc_addr.str();

        if (zmq_bind(sockets[i], ipc.c_str()) != 0) {
            throw runtime_error(strerror (errno));
        }
    }


    auto metadata_buffer = make_unique<ModuleFrame>();
    auto image_buffer = make_unique<uint16_t[]>(32*512*1024);

    int i_write = 0;
    size_t total_ms = 0;

    while (true) {
        uint64_t pulse_id = 0;

        auto start_time = chrono::steady_clock::now();

        for (size_t i=0; i<n_modules; i++) {
            auto n_bytes_metadata = zmq_recv(
                    sockets[i],
                    metadata_buffer.get(),
                    sizeof(ModuleFrame),
                    0);

            if (n_bytes_metadata != sizeof(ModuleFrame)) {
                throw runtime_error(strerror (errno));

            }

            if (i == 0) {
                pulse_id = metadata_buffer->pulse_id;
            }

            if (pulse_id != metadata_buffer->pulse_id) {
                cout << "Module " << i << " pulse " << metadata_buffer->pulse_id;
                cout << " instead of " << pulse_id << endl;
            }

            auto n_bytes_image = zmq_recv(
                    sockets[i],
                    (image_buffer.get() + (512*1024*i)),
                    512 * 1024 * 2,
                    0);

            if (n_bytes_image != 512 * 1024 * 2) {
                cout << "n_bytes_image " << n_bytes_image << endl;
                throw runtime_error("Unexpected number of bytes in image.");
            }
        }

//        writer.write_data("image", i_write, (char*) (image_buffer.get()),
//                {32*512, 1024}, 32*512*1024*2, "uint16", "little");
        i_write++;

        auto end_time = chrono::steady_clock::now();

        // TODO: Some poor statistics.

        auto ms_duration = chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count();
        total_ms += ms_duration;

        if (i_write==100) {
            cout << "assembly_ms " << total_ms / 100 << endl;
            i_write = 0;
        }
    }

    writer.close_file();

    for (size_t i=0; i<n_modules; i++) {
        zmq_close(sockets[i]);
    }

    zmq_ctx_destroy(ctx);

    return 0;
}
