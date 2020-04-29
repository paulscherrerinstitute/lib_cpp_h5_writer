#include <iostream>
#include <stdexcept>
#include "buffer_config.hpp"
#include "zmq.h"
#include <string>
#include <jungfrau.hpp>
#include <thread>
#include <chrono>
#include "SFWriter.hpp"
#include <FastQueue.hpp>

using namespace std;
using namespace core_buffer;

int main (int argc, char *argv[])
{
    if (argc != 4) {
        cout << endl;
        cout << "Usage: sf_writer ";
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

    string ipc_prefix = "ipc://sf-replay-";
    auto ctx = zmq_ctx_new();
    zmq_ctx_set (ctx, ZMQ_IO_THREADS, WRITER_ZMQ_IO_THREADS);

    void *sockets[n_modules];
    for (size_t i = 0; i < n_modules; i++) {
        sockets[i] = zmq_socket(ctx, ZMQ_PULL);
        int rcvhwm = REPLAY_BLOCK_SIZE;
        if (zmq_setsockopt(sockets[i], ZMQ_RCVHWM, &rcvhwm,
                           sizeof(rcvhwm)) != 0) {
            throw runtime_error(strerror(errno));
        }
        int linger = 0;
        if (zmq_setsockopt(sockets[i], ZMQ_LINGER, &linger,
                           sizeof(linger)) != 0) {
            throw runtime_error(strerror(errno));
        }

        stringstream ipc_addr;
        ipc_addr << ipc_prefix << i;
        const auto ipc = ipc_addr.str();

        if (zmq_bind(sockets[i], ipc.c_str()) != 0) {
            throw runtime_error(strerror(errno));
        }
    }

    size_t n_frames = stop_pulse_id - start_pulse_id;
    SFWriter writer(output_file, n_frames, n_modules);

    // TODO: Remove stats trash.
    int stats_counter = 0;

    size_t read_total_us = 0;
    size_t write_total_us = 0;
    size_t read_max_us = 0;
    size_t write_max_us = 0;

    auto module_meta_buffer = make_unique<ModuleFrame>();
    auto frame_meta_buffer = make_unique<DetectorFrame>();
    auto frame_buffer = make_unique<char[]>(MODULE_N_BYTES * n_modules);

    auto start_time = chrono::steady_clock::now();

    auto current_pulse_id = start_pulse_id;
    while (current_pulse_id <= stop_pulse_id) {

        for (size_t i = 0; i < n_modules; i++) {
            auto n_bytes_metadata = zmq_recv(
                    sockets[i],
                    module_meta_buffer.get(),
                    sizeof(ModuleFrame),
                    0);

            if (n_bytes_metadata != sizeof(ModuleFrame)) {
                // TODO: Make nicer expcetion.
                throw runtime_error(strerror(errno));
            }

            // Initialize buffers in first iteration for each pulse_id.
            if (i == 0) {
                frame_meta_buffer->pulse_id =
                        module_meta_buffer->pulse_id;
                frame_meta_buffer->frame_index =
                        module_meta_buffer->frame_index;
                frame_meta_buffer->daq_rec =
                        module_meta_buffer->daq_rec;
                frame_meta_buffer->n_received_packets =
                        module_meta_buffer->n_received_packets;
            }

            if (frame_meta_buffer->pulse_id !=
                module_meta_buffer->pulse_id) {
                throw runtime_error("Unexpected pulse_id received.");
            }

            auto n_bytes_image = zmq_recv(
                    sockets[i],
                    ((frame_buffer.get()) + (MODULE_N_PIXELS * i)),
                    MODULE_N_BYTES,
                    0);

            if (n_bytes_image != MODULE_N_BYTES) {
                // TODO: Make nicer exception.
                throw runtime_error("Unexpected number of bytes in image.");
            }
        }

        auto read_end_time = chrono::steady_clock::now();
        auto read_us_duration = chrono::duration_cast<chrono::microseconds>(
                read_end_time-start_time).count();

        start_time = chrono::steady_clock::now();

        writer.write(frame_meta_buffer.get(), frame_buffer.get());
        current_pulse_id++;

        // TODO: Some poor statistics.
        stats_counter++;
        auto write_end_time = chrono::steady_clock::now();
        auto write_us_duration = chrono::duration_cast<chrono::microseconds>(
                write_end_time-start_time).count();

        read_total_us += read_us_duration;
        write_total_us += write_us_duration;

        if (read_us_duration > read_max_us) {
            read_max_us = read_us_duration;
        }

        if (write_us_duration > write_max_us) {
            write_max_us = write_us_duration;
        }

        if (stats_counter == STATS_MODULO) {
            cout << "sf_writer:read_us " << read_total_us / STATS_MODULO;
            cout << " sf_writer:read_max_us " << read_max_us;
            cout << " sf_writer:write_us " << write_total_us / STATS_MODULO;
            cout << " sf_writer:write_max_us " << write_max_us;

            cout << endl;

            stats_counter = 0;
            read_total_us = 0;
            read_max_us = 0;
            write_total_us = 0;
            write_max_us = 0;
        }

        start_time = chrono::steady_clock::now();
    }

    writer.close_file();

    for (size_t i = 0; i < n_modules; i++) {
        zmq_close(sockets[i]);
    }
    zmq_ctx_destroy(ctx);

    return 0;
}
