#include <iostream>
#include <zmq.hpp>
#include <cstdlib>
#include <thread>
#include "rapidjson/document.h"

#include "WriterManager.hpp"
#include "H5ChunkedWriter.hpp"
#include "RingBuffer.hpp"

using namespace std;

void write(RingBuffer *ring_buffer, string output_file, WriterManager *manager) 
{
    string dataset_name = "data";
    HDF5ChunkedWriter writer(output_file, dataset_name);

    // Run until the running flag is set or the ring_buffer is empty.  
    while(manager->is_running() || !ring_buffer->is_empty()) {
        pair<FrameMetadata, char*> received_data = ring_buffer->read();

        writer.write_data(received_data.first.frame_index, 
                          received_data.first.frame_shape, 
                          received_data.first.frame_bytes_size, 
                          received_data.second);

        ring_buffer->release(received_data.first.buffer_slot_index);

        manager->written_frame(received_data.first.frame_index);
    }

    writer.close_file();
}

void receive(string connect_address, uint64_t n_images, int n_slots=100, int n_io_threads=1)
{
    WriterManager manager(n_images);
    RingBuffer ring_buffer(n_slots);

    thread writer_thread(write, &ring_buffer, "output.h5", &manager);

    zmq::context_t context(n_io_threads);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.connect(connect_address);
    
    zmq::message_t message_data;
    FrameMetadata frame_metadata;

    rapidjson::Document header_parser;

    while (manager.is_running()) {
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

        // Commit the frame to the buffer.
        ring_buffer.write(frame_metadata, static_cast<char*>(message_data.data()));

        manager.received_frame(frame_metadata.frame_index);
   }

   writer_thread.join();
}

int main (int argc, char *argv[])
{
    if (argc != 3) {
        cout << "Usage: h5_zmq_writer [connection_address] [n_images]" << endl;
        cout << "\tconnection_address: Address to connect to the stream (PULL). Example: tcp://127.0.0.1:40000" << endl;
        cout << "\tn_images: Number of images to acquire. 0 for infinity (untill STOP is called)." << endl;
        exit(-1);
    }

    receive(string(argv[1]), atoi(argv[2]));
    return 0;
}
