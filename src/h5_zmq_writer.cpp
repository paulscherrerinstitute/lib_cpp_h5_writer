#include <iostream>
#include <zmq.hpp>
#include <atomic>
#include "rapidjson/document.h"
#include <cstdlib>
#include "RingBuffer.hpp"
#include "H5ChunkedWriter.hpp"

using namespace std;

void write(RingBuffer *ring_buffer, string output_file, atomic_bool* running_flag) 
{
    string dataset_name = "data";
    HDF5ChunkedWriter writer(output_file, dataset_name);

    // Run until the running flag is set or the ring_buffer is empty.  
    while(*running_flag || !ring_buffer->is_empty()) {
        pair<FrameMetadata, char*> received_data = ring_buffer->read();

        writer.write_data(received_data.first.frame_index, 
                          received_data.first.frame_shape, 
                          received_data.first.frame_bytes_size, 
                          received_data.second);

        ring_buffer->release(received_data.first.buffer_slot_index);
    }

    writer.close_file();
}

void receive(int num_io_threads, string connect_address, uint64_t n_images, int n_slots=100)
{
    bool ring_buffer_initialized = false;
    RingBuffer ring_buffer(n_slots);
    atomic_bool running_flag(true);

    thread writer_thread(write, &ring_buffer, "output.h5", &running_flag);

    zmq::context_t context(num_io_threads);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.connect(connect_address);

    // int has_more = 0;
    // size_t has_more_size = sizeof(has_more);
    
    zmq::message_t message_data;
    FrameMetadata frame_metadata;

    rapidjson::Document header_parser;

    for (uint64_t i=0; i<n_images; i++) {
        // Get the message header.
        receiver.recv(&message_data);

        // Parse JSON header.
        char* header = static_cast<char*>(message_data.data());
        header_parser.Parse(header);

        // Extract frame_index and frame_shape from the message header.
        frame_metadata.frame_index = header_parser["frame"].GetUint64();

        auto header_shape = header_parser["shape"].GetArray();
        frame_metadata.frame_shape[0] = header_shape[0].GetUint64();
        frame_metadata.frame_shape[1] = header_shape[1].GetUint64();
        
        // Get the message data.
        receiver.recv(&message_data);
        frame_metadata.frame_bytes_size = message_data.size(); 

        if (!ring_buffer_initialized) {
            ring_buffer.initialize(frame_metadata.frame_bytes_size);
            ring_buffer_initialized = true;
        }

        // Commit the frame to the buffer.
        ring_buffer.write(frame_metadata, static_cast<char*>(message_data.data()));
   }

   running_flag = false;

   writer_thread.join();
}

int main (int argc, char *argv[])
{
    if (argc != 2) {
        cout << "Usage: h5_zmq_writer [n_images]" << endl;
        exit(-1);
    }

    int num_io_threads = 1;
    receive(num_io_threads, "tcp://127.0.0.1:40000", atoi(argv[1]));
    return 0;
}
