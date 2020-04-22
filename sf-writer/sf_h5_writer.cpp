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

//    H5Writer writer(output_file);
//    writer.create_file();

    size_t n_modules = 32;

    auto metadata_buffer = make_unique<ModuleFrame>();
    auto image_buffer = make_unique<uint16_t[]>(n_modules*512*1024);

    uint32_t all_modules_ready = 0;
    for (size_t i=0; i<n_modules; i++) {
        all_modules_ready |= 1 << i;
    }

    atomic_uint32_t modules_ready = all_modules_ready;

    auto ctx = zmq_ctx_new();
    zmq_ctx_set (ctx, ZMQ_IO_THREADS, 16);

    vector<thread> threads;

    auto read_thread = [&](int module_id) {
        auto socket = zmq_socket(ctx, ZMQ_PULL);

        int rcvhwm = 100;
        if (zmq_setsockopt(socket, ZMQ_RCVHWM, &rcvhwm, sizeof(rcvhwm)) != 0)
            throw runtime_error(strerror (errno));

        int linger = 0;
        if (zmq_setsockopt(socket, ZMQ_LINGER, &linger, sizeof(linger)) != 0)
            throw runtime_error(strerror (errno));

        auto ipc_address = "ipc://sf-replay-" + to_string(module_id);
        if (zmq_bind(socket, ipc_address.c_str()) != 0)
            throw runtime_error(strerror (errno));

        uint32_t module_mask = 0;
        module_mask |= 1 << module_id;

        while (true) {
            if ((modules_ready & module_mask) == 0) {
                continue;
            }

            auto n_bytes_metadata = zmq_recv(
                    socket,
                    metadata_buffer.get(),
                    sizeof(ModuleFrame),
                    0);

            if (n_bytes_metadata != sizeof(ModuleFrame))
                throw runtime_error(strerror (errno));

            auto image_buffer_offset = 512*1024*module_id;
            auto n_bytes_image = zmq_recv(
                    socket,
                    image_buffer.get() + image_buffer_offset,
                    512 * 1024 * 2,
                    0);

            if (n_bytes_image != 512 * 1024 * 2) {
                throw runtime_error(strerror (errno));
            }

            modules_ready ^= 1 << module_id;
        }
    };

    for (size_t i=0; i<n_modules; i++) {
        threads.emplace_back(read_thread, i);
    }

    int i_write = 0;
    size_t total_ms = 0;
    size_t max_ms = 0;

    auto start_time = chrono::steady_clock::now();

    while (true) {

        if (modules_ready != 0) {
            continue;
        }

        i_write++;

        auto end_time = chrono::steady_clock::now();

        auto ms_duration = chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count();
        total_ms += ms_duration;
        if (ms_duration > max_ms) {
            max_ms = ms_duration;
        }

        if (i_write==100) {
            cout << "assembly_ms " << total_ms / 100;
            cout << "max_ms " << max_ms << endl;
            i_write = 0;
            total_ms = 0;
            max_ms = 0;
        }
        auto start_time = chrono::steady_clock::now();
    }

    return 0;
}
