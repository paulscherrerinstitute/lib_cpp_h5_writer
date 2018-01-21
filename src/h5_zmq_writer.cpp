#include <iostream>
#include <sstream>
#include <zmq.hpp>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <boost/property_tree/json_parser.hpp>

#include "config.hpp"
#include "WriterManager.hpp"
#include "H5Writer.hpp"
#include "RingBuffer.hpp"
#include "rest_interface.hpp"
#include "h5_utils.hpp"

using namespace std;
namespace pt = boost::property_tree;

void write_h5(WriterManager *manager, RingBuffer *ring_buffer, string output_file) 
{
    H5Writer writer(output_file, "data");

    // Run until the running flag is set or the ring_buffer is empty.  
    while(manager->is_running() || !ring_buffer->is_empty()) {
        
        if (ring_buffer->is_empty()) {
            this_thread::sleep_for(chrono::milliseconds(config::ring_buffer_read_retry_interval));
            continue;
        }

        pair<FrameMetadata, char*> received_data = ring_buffer->read();
        
        // NULL pointer means that the ringbuffer->read() timeouted. Faster than rising an exception.
        if(!received_data.second) {
            continue;
        }

        writer.write_frame_data(received_data.first.frame_index, 
                                received_data.first.frame_shape,
                                received_data.first.frame_bytes_size, 
                                received_data.second,
                                received_data.first.type,
                                received_data.first.endianness);

        ring_buffer->release(received_data.first.buffer_slot_index);

        manager->written_frame(received_data.first.frame_index);
    }

    if (writer.is_file_open()) {
        #ifdef DEBUG_OUTPUT
            cout << "[h5_zmq_writer::write] Writing file format." << endl;
        #endif

        h5_utils::write_format(writer.get_h5_file(), manager->get_parameters());
    }
    
    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::write] Closing file " << output_file << endl;
    #endif
    
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

        #ifdef DEBUG_OUTPUT
            cout << "[h5_zmq_writer::receive_zmq] Processing FrameMetadata"; 
            cout << " with frame_index " << frame_metadata.frame_index;
            cout << " and frame_shape [" << frame_metadata.frame_shape[0] << ", " << frame_metadata.frame_shape[1] << "]";
            cout << " and endianness " << frame_metadata.endianness;
            cout << " and type " << frame_metadata.type;
            cout << " and frame_bytes_size " << frame_metadata.frame_bytes_size;
            cout << "." << endl;
        #endif

        // Commit the frame to the buffer.
        ring_buffer->write(frame_metadata, static_cast<char*>(message_data.data()));

        manager->received_frame(frame_metadata.frame_index);
   }

    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::receive_zmq] Receiver thread stopped." << endl;
    #endif
}

void run_writer(string connect_address, string output_file, uint64_t n_images, uint16_t rest_port){

    size_t n_slots = config::ring_buffer_n_slots;
    int n_io_threads = config::zmq_n_io_threads;
    int receive_timeout = config::zmq_receive_timeout;

    WriterManager manager(n_images);
    RingBuffer ring_buffer(n_slots);

    // TODO: Remove this. This is needed only for testing.
    std::map<std::string, boost::any> parameters = {};
    parameters.insert({"sl2wv", 1.0});
    parameters.insert({"sl0ch", 1.0});
    parameters.insert({"sl2wh", 1.0});
    parameters.insert({"temp_mono_cryst_1", 1.0});
    parameters.insert({"harmonic", 2});
    parameters.insert({"mokev", 1.0});
    parameters.insert({"sl2cv", 1.0});
    parameters.insert({"bpm4_gain_setting", 1.0});
    parameters.insert({"mirror_coating", string("test_char")});
    parameters.insert({"samx", 1.0});
    parameters.insert({"sample_name", string("test_char")});
    parameters.insert({"bpm5y", 1.0});
    parameters.insert({"sl2ch", 1.0});
    parameters.insert({"curr", 1.0});
    parameters.insert({"bs2_status", string("test_char")});
    parameters.insert({"bs2y", 1.0});
    parameters.insert({"diode", 1.0});
    parameters.insert({"samy", 1.0});
    parameters.insert({"sl4ch", 1.0});
    parameters.insert({"sl4wh", 1.0});
    parameters.insert({"temp_mono_cryst_2", 1.0});
    parameters.insert({"sl3wh", 1.0});
    parameters.insert({"mith", 1.0});
    parameters.insert({"bs1_status", string("test_char")});
    parameters.insert({"bpm4s", 1.0});
    parameters.insert({"sl0wh", 1.0});
    parameters.insert({"bpm6z", 1.0});
    parameters.insert({"bs1y", 1.0});
    parameters.insert({"scan", string("test_char")});
    parameters.insert({"bpm5_gain_setting", 1.0});
    parameters.insert({"bpm4z", 1.0});
    parameters.insert({"bpm4x", 1.0});
    parameters.insert({"date", string("test_char")});
    parameters.insert({"mibd", 1.0});
    parameters.insert({"temp", 1.0});
    parameters.insert({"idgap", 1.0});
    parameters.insert({"sl4cv", 1.0});
    parameters.insert({"sl1wv", 1.0});
    parameters.insert({"sl3wv", 1.0});
    parameters.insert({"sl1ch", 1.0});
    parameters.insert({"bs2x", 1.0});
    parameters.insert({"bpm6_gain_setting", 1.0});
    parameters.insert({"bpm4y", 1.0});
    parameters.insert({"bpm6s", 1.0});
    parameters.insert({"sample_description", string("test_char")});
    parameters.insert({"bpm5z", 1.0});
    parameters.insert({"moth1", 1.0});
    parameters.insert({"sec", 1.0});
    parameters.insert({"sl3cv", 1.0});
    parameters.insert({"bs1x", 1.0});
    parameters.insert({"bpm6_saturation_value", 1.0});
    parameters.insert({"bpm5s", 1.0});
    parameters.insert({"mobd", 1.0});
    parameters.insert({"sl1wh", 1.0});
    parameters.insert({"sl4wv", 1.0});
    parameters.insert({"bs2_det_dist", 1.0});
    parameters.insert({"bpm5_saturation_value", 1.0});
    parameters.insert({"fil_comb_description", string("test_char")});
    parameters.insert({"bpm5x", 1.0});
    parameters.insert({"bpm4_saturation_value", 1.0});
    parameters.insert({"bs1_det_dist", 1.0});
    parameters.insert({"sl3ch", 1.0});
    parameters.insert({"bpm6y", 1.0});
    parameters.insert({"sl1cv", 1.0});
    parameters.insert({"bpm6x", 1.0});
    parameters.insert({"ftrans", 1.0});
    parameters.insert({"samz", 1.0});
    manager.set_parameters(parameters);

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
