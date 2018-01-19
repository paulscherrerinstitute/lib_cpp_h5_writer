#include <iostream>
#include <sstream>
#include <zmq.hpp>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <boost/property_tree/json_parser.hpp>
 

#include "config.hpp"
#include "WriterManager.hpp"
#include "H5ChunkedWriter.hpp"
#include "RingBuffer.hpp"
#include "rest_interface.hpp"
#include "h5_utils.hpp"



using namespace std;
namespace pt = boost::property_tree;

void write_h5(WriterManager *manager, RingBuffer *ring_buffer, string output_file) 
{
    HDF5ChunkedWriter writer(output_file, "data");

    // Run until the running flag is set or the ring_buffer is empty.  
    while(manager->is_running() || !ring_buffer->is_empty()) {
        
        if (ring_buffer->is_empty()) {
            this_thread::sleep_for(chrono::milliseconds(config::ring_buffer_read_retry_interval));
            continue;
        }

        pair<FrameMetadata, char*> received_data = ring_buffer->read();

        writer.write_data(received_data.first.frame_index, 
                          received_data.first.frame_shape, 
                          received_data.first.frame_bytes_size, 
                          received_data.second,
                          received_data.first.type,
                          received_data.first.endianness);

        ring_buffer->release(received_data.first.buffer_slot_index);

        manager->written_frame(received_data.first.frame_index);
    }

    h5_utils::write_format(writer.get_h5_file(), manager->get_parameters());

    writer.close_file();

    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::write] Writer thread stopped." << endl;
    #endif

    exit(0);
}

void receive_zmq(WriterManager *manager, RingBuffer *ring_buffer, string connect_address, int n_io_threads=1, int receive_timeout=-1)
{
    zmq::context_t context(n_io_threads);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.setsockopt(ZMQ_RCVTIMEO, receive_timeout);
    receiver.connect(connect_address);
    
    zmq::message_t message_header(config::zmq_buffer_size_header);
    zmq::message_t message_data(config::zmq_buffer_size_data);

    FrameMetadata frame_metadata;

    pt::ptree json_header;

    while (manager->is_running()) {
        // Get the message header.
        if (!receiver.recv(&message_header)){
            continue;
        }

        // Parse JSON header.
        string header_string(static_cast<char*>(message_header.data()), message_header.size());
        stringstream header_stream;
        header_stream << header_string << endl;
        pt::read_json(header_stream, json_header);

        // Extract data from message header.
        frame_metadata.frame_index = json_header.get<uint64_t>("frame");

        uint8_t index = 0;
        for (auto item : json_header.get_child("shape")) {
            frame_metadata.frame_shape[index] = item.second.get_value<size_t>();
            index++;
        }
        
        // Array 1.0 specified little endian as the default encoding.
        frame_metadata.endianness = json_header.get("endianness", "little");

        frame_metadata.type = json_header.get<string>("type");

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

    size_t n_slots = config::ring_buffer_n_slots;
    int n_io_threads = config::zmq_n_io_threads;
    int receive_timeout = config::zmq_receive_timeout;

    WriterManager manager(n_images);
    RingBuffer ring_buffer(n_slots);

    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::run_writer] Running writer"; 
        cout << " with connect_address " << connect_address;
        cout << " and output_file " << output_file;
        cout << " and n_slots " << n_slots;
        cout << " and n_io_threads " << n_io_threads;
        cout << " and receive_timeout " << receive_timeout;
        cout << endl;
    #endif

    thread receiver_thread(receive_zmq, &manager, &ring_buffer, connect_address, n_io_threads, receive_timeout);
    thread writer_thread(write_h5, &manager, &ring_buffer, output_file);

    start_rest_api(manager, rest_port, get_input_value_type());

    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::run_writer] Rest API stopped." << endl;
    #endif

    // In case SIGINT stopped the rest_api.
    manager.stop();

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
        cout << "\trest_port: Port to start the REST Api on." << endl;
        cout << endl;

        exit(-1);
    }

    run_writer(string(argv[1]), string(argv[2]), atoi(argv[3]), atoi(argv[4]));

    return 0;
}
