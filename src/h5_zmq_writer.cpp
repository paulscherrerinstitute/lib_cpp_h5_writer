#include <iostream>
#include <zmq.hpp>
#include <cstdlib>
#include <thread>
#include "rapidjson/document.h"

#include "config.hpp"
#include "WriterManager.hpp"
#include "H5ChunkedWriter.hpp"
#include "RingBuffer.hpp"
#include "rest_interface.hpp"

using namespace std;

void write(WriterManager *manager, RingBuffer *ring_buffer, string output_file) 
{
    HDF5ChunkedWriter writer(output_file, config::dataset_name);

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

    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::write] Writer thread stopped." << endl;
    #endif
}

void receive(WriterManager *manager, RingBuffer *ring_buffer, string connect_address, int n_io_threads=1)
{
    zmq::context_t context(n_io_threads);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.connect(connect_address);
    
    zmq::message_t message_data;
    FrameMetadata frame_metadata;

    rapidjson::Document header_parser;

    while (manager->is_running()) {
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
        ring_buffer->write(frame_metadata, static_cast<char*>(message_data.data()));

        manager->received_frame(frame_metadata.frame_index);
   }

    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::receive] Receiver thread stopped." << endl;
    #endif
}

void run_writer(string connect_address, string output_file, uint64_t n_images, uint16_t rest_port){

    size_t n_slots = config::n_slots;
    int n_io_threads = config::n_io_threads;

    WriterManager manager(n_images);
    RingBuffer ring_buffer(n_slots);

    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::run_writer] Running writer"; 
        cout << " with connect_address " << connect_address;
        cout << " and output_file " << output_file;
        cout << " and n_slots " << n_slots;
        cout << " and n_io_threads " << n_io_threads;
        cout << endl;
    #endif

    thread receiver_thread(receive, &manager, &ring_buffer, connect_address, n_io_threads);
    thread writer_thread(write, &manager, &ring_buffer, output_file);

    start_rest_api(manager, rest_port);

    receiver_thread.join();
    writer_thread.join();

    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::run_writer] Writer properly stopped." << endl;
    #endif
}

int main (int argc, char *argv[])
{
    if (argc != 5) {
        cout << endl;
        cout << "Usage: h5_zmq_writer [connection_address] [output_file] [n_images] [rest_port]" << endl;
        cout << "\tconnection_address: Address to connect to the stream (PULL). Example: tcp://127.0.0.1:40000" << endl;
        cout << "\toutput_file: Name of the output file." << endl;
        cout << "\tn_images: Number of images to acquire. 0 for infinity (untill /stop is called)." << endl;
        cout << "\rest_port: Port to start the REST Api on." << endl;
        cout << endl;

        exit(-1);
    }

    run_writer(string(argv[1]), string(argv[2]), atoi(argv[3]), atoi(argv[4]));

    return 0;
}
