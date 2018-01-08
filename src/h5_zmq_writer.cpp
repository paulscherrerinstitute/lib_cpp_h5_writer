#include <iostream>
#include <zmq.hpp>
#include <stdexcept>
#include <csignal>
#include <atomic>

#include "RingBuffer.hpp"
#include "H5ChunkedWriter.hpp"

using namespace std;

void write(RingBuffer *ring_buffer, string output_file) 
{
    string dataset_name = "data";
    HDF5ChunkedWriter writer(output_file, dataset_name);

    while (true) {
        pair<FrameMetadata, char*> received_data = ring_buffer->read();
        
        writer.write_data(received_data.first.frame_index, 
                          received_data.first.frame_shape, 
                          received_data.first.frame_bytes_size, 
                          received_data.second);

        ring_buffer->release(received_data.first.buffer_slot_index);
    }

    writer.close_file();
}

void receive(int num_io_threads, string connect_address, int n_slots=100)
{
    bool ring_buffer_initialized = false;
    RingBuffer ring_buffer(n_slots);

    thread writer_thread(write, &ring_buffer, "output.h5");

    zmq::context_t context(num_io_threads);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.connect(connect_address);

    // int has_more = 0;
    // size_t has_more_size = sizeof(has_more);
    
    zmq::message_t message_data;
    FrameMetadata frame_metadata;

    for (int i=0; i<10; i++) {
        // Get the message header.
        receiver.recv(&message_data);
        string header = string(static_cast<char*>(message_data.data()), message_data.size());
        frame_metadata.frame_header = header;

        // Extract frame_index and frame_shape from the message header.
        frame_metadata.frame_index = i;
        frame_metadata.frame_shape[0] = 1;
        frame_metadata.frame_shape[1] = 10;
        
        // Get the message data.
        receiver.recv(&message_data);
        frame_metadata.frame_bytes_size = message_data.size(); 

        if (!ring_buffer_initialized) {
            ring_buffer.initialize(frame_metadata.frame_bytes_size);
        }

        // Commit the frame to the buffer.
        ring_buffer.write(frame_metadata, static_cast<char*>(message_data.data()));
   }
    
}


int main (int argc, char *argv[])
{
    int num_io_threads = 1;
    receive(num_io_threads, "tcp://127.0.0.1:40000");
    return 0;
}
