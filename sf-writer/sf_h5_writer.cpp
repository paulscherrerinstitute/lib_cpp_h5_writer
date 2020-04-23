#include <iostream>
#include <stdexcept>
#include "buffer_config.hpp"
#include "zmq.h"
#include <string>
#include <RingBuffer.hpp>
#include <jungfrau.hpp>
#include <thread>
#include <chrono>
#include <H5Writer.hpp>
#include <config.hpp>

using namespace std;
using namespace core_buffer;


void receive_replay(
        const string ipc_prefix,
        const size_t n_modules,
        RingBuffer<DetectorFrame>& ring_buffer,
        void* ctx)
{


    void* sockets[n_modules];
    for (size_t i=0; i<n_modules; i++) {
        sockets[i] = zmq_socket(ctx, ZMQ_PULL);
        int rcvhwm = REPLAY_BLOCK_SIZE;
        if (zmq_setsockopt(sockets[i], ZMQ_RCVHWM, &rcvhwm, sizeof(rcvhwm)) != 0) {
            throw runtime_error(strerror (errno));
        }
        int linger = 0;
        if (zmq_setsockopt(sockets[i], ZMQ_LINGER, &linger, sizeof(linger)) != 0) {
            throw runtime_error(strerror (errno));
        }

        stringstream ipc_addr;
        ipc_addr << ipc_prefix << i;
        auto ipc = ipc_addr.str();

        if (zmq_bind(sockets[i], ipc.c_str()) != 0) {
            throw runtime_error(strerror (errno));
        }
    }

    auto metadata_buffer = make_unique<ModuleFrame>();
    char* image_buffer = nullptr;

    while (true) {
        auto rb_metadata = make_shared<DetectorFrame>();
        image_buffer = ring_buffer.reserve(rb_metadata);

        for (size_t i=0; i<n_modules; i++) {
            auto n_bytes_metadata = zmq_recv(
                    sockets[i],
                    metadata_buffer.get(),
                    sizeof(ModuleFrame),
                    0);

            if (n_bytes_metadata != sizeof(ModuleFrame)) {
                // TODO: Make nicer expcetion.
                throw runtime_error(strerror (errno));
            }

            // Initialize buffers in first iteration for each pulse_id.
            if (i == 0) {
                rb_metadata->pulse_id = metadata_buffer->pulse_id;
                rb_metadata->frame_index = metadata_buffer->frame_index;
                rb_metadata->daq_rec = metadata_buffer->daq_rec;
                rb_metadata->n_received_packets =
                        metadata_buffer->n_received_packets;
            }

            if (rb_metadata->pulse_id != metadata_buffer->pulse_id) {
                throw runtime_error("Unexpected pulse_id received.");
            }

            auto n_bytes_image = zmq_recv(
                    sockets[i],
                    (image_buffer + (MODULE_N_PIXELS*i)),
                    MODULE_N_BYTES,
                    0);

            if (n_bytes_image != MODULE_N_BYTES) {
                // TODO: Make nicer expcetion.
                throw runtime_error("Unexpected number of bytes in image.");
            }
        }

        ring_buffer.commit(rb_metadata);
    }

    for (size_t i=0; i<n_modules; i++) {
        zmq_close(sockets[i]);
    }

    zmq_ctx_destroy(ctx);
}

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

    RingBuffer<DetectorFrame> ring_buffer(5);

    size_t n_modules = 32;
    string ipc_prefix = "ipc://sf-replay-";
    auto ctx = zmq_ctx_new();
    zmq_ctx_set (ctx, ZMQ_IO_THREADS, WRITER_ZMQ_IO_THREADS);

    thread replay_receive_thread(
            receive_replay,
            ipc_prefix,
            n_modules,
            ref(ring_buffer),
            ctx);

    H5Writer writer(output_file);
    writer.create_file();

    // TODO: Remove stats trash.
    int i_write = 0;
    size_t total_ms = 0;
    size_t max_ms = 0;

    for (
            size_t current_pulse_id=start_pulse_id;
            current_pulse_id <= stop_pulse_id;
            current_pulse_id++)
    {
        auto start_time = chrono::steady_clock::now();

        auto received_data = ring_buffer.read();

        // .first is nullptr if ringbuffer is empty.
        if(received_data.first == nullptr) {
            this_thread::sleep_for(chrono::milliseconds(
                    config::ring_buffer_read_retry_interval));

            // TODO: Very ugly hack. Make it nicer.
            current_pulse_id--;
            continue;
        }

        auto metadata = received_data.first;
        auto data = received_data.second;

        cout << "Received pulse_id " << metadata->pulse_id << endl;

        if (metadata->pulse_id != current_pulse_id) {
            cout << "ERROR expecting " << current_pulse_id << endl;
        }

        // TODO: Write to H5

        i_write++;

        auto end_time = chrono::steady_clock::now();

        // TODO: Some poor statistics.

        auto ms_duration = chrono::duration_cast<chrono::milliseconds>(end_time-start_time).count();
        total_ms += ms_duration;
        if (ms_duration > max_ms) {
            max_ms = ms_duration;
        }

        if (i_write==100) {
            cout << "assembly_ms " << total_ms / 100;
            cout << " max_ms " << max_ms << endl;
            i_write = 0;
            total_ms = 0;
            max_ms = 0;
        }

    }

    writer.close_file();

    return 0;
}
