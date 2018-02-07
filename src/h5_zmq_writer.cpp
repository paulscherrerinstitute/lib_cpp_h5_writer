#include <iostream>
#include <sstream>
#include <zmq.hpp>
#include <cstdlib>
#include <chrono>
#include <unistd.h>
#include <stdexcept>
#include <boost/property_tree/json_parser.hpp>
#include <boost/thread.hpp>

#include "config.hpp"
#include "WriterManager.hpp"
#include "H5Writer.hpp"
#include "RingBuffer.hpp"
#include "rest_interface.hpp"
#include "H5Format.hpp"

#include "format/NXmxFormat.cpp"

using namespace std;
namespace pt = boost::property_tree;

void write_h5(WriterManager& manager, const H5Format& format, RingBuffer& ring_buffer) 
{
    H5Writer writer(manager.get_output_file(), format.get_raw_frames_dataset_name());

    // Run until the running flag is set or the ring_buffer is empty.  
    while(manager.is_running() || !ring_buffer.is_empty()) {
        
        if (ring_buffer.is_empty()) {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(config::ring_buffer_read_retry_interval));
            continue;
        }

        const pair<FrameMetadata, char*> received_data = ring_buffer.read();
        
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

        ring_buffer.release(received_data.first.buffer_slot_index);

        manager.written_frame(received_data.first.frame_index);
    }

    if (writer.is_file_open()) {
        #ifdef DEBUG_OUTPUT
            cout << "[h5_zmq_writer::write] Writing file format." << endl;
        #endif

        // Wait until all parameters are set or writer is killed.
        while (!manager.are_all_parameters_set() && !manager.is_killed()) {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(config::parameters_read_retry_interval));
        }

        // Need to check again if we have all parameters to write down the format.
        if (manager.are_all_parameters_set()) {
            const auto parameters = manager.get_parameters();
            
            // Even if we can't write the format, lets try to preserve the data.
            try {
                H5FormatUtils::write_format(writer.get_h5_file(), format, parameters);
            } catch (const runtime_error& ex) {
                cerr << "[h5_zmq_writer::write] Error while trying to write file format: "<< ex.what() << endl;
            }
        }
    }
    
    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::write] Closing file " << manager.get_output_file() << endl;
    #endif
    
    writer.close_file();

    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::write] Writer thread stopped." << endl;
    #endif

    // Exit when writer thread has closed the file.
    exit(0);
}

void receive_zmq(WriterManager& manager, RingBuffer& ring_buffer, const string connect_address, int n_io_threads=1, int receive_timeout=-1)
{
    zmq::context_t context(n_io_threads);
    zmq::socket_t receiver(context, ZMQ_PULL);
    receiver.setsockopt(ZMQ_RCVTIMEO, receive_timeout);
    receiver.connect(connect_address);
    
    zmq::message_t message_header(config::zmq_buffer_size_header);
    zmq::message_t message_data(config::zmq_buffer_size_data);

    FrameMetadata frame_metadata;

    pt::ptree json_header;

    while (manager.is_running()) {
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
        for (const auto& item : json_header.get_child("shape")) {
            frame_metadata.frame_shape[index] = item.second.get_value<size_t>();
            ++index;
        }

        // Array 1.0 specified little endian as the default encoding.
        frame_metadata.endianness = json_header.get("endianness", "little");

        frame_metadata.type = json_header.get<string>("type");

        // Get the message data.
        if (!receiver.recv(&message_data)) {
            cout << "[h5_zmq_writer::receive_zmq] ERROR: Error while reading from ZMQ. Frame index " << frame_metadata.frame_index << " lost."; 
            cout << " Trying to continue with the next frame." << endl;

            manager.lost_frame(frame_metadata.frame_index);

            continue;
        }

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
        ring_buffer.write(frame_metadata, static_cast<char*>(message_data.data()));

        manager.received_frame(frame_metadata.frame_index);
   }

    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::receive_zmq] Receiver thread stopped." << endl;
    #endif
}

void run_writer(WriterManager& manager, const H5Format& format, const string& connect_address, uint16_t rest_port)
{
    size_t n_slots = config::ring_buffer_n_slots;
    int n_io_threads = config::zmq_n_io_threads;
    int receive_timeout = config::zmq_receive_timeout;

    RingBuffer ring_buffer(n_slots);

    #ifdef DEBUG_OUTPUT
        cout << "[h5_zmq_writer::run_writer] Running writer"; 
        cout << " with connect_address " << connect_address;
        cout << " and output_file " << manager.get_output_file();
        cout << " and n_slots " << n_slots;
        cout << " and n_io_threads " << n_io_threads;
        cout << " and receive_timeout " << receive_timeout;
        cout << endl;
    #endif

    boost::thread receiver_thread(receive_zmq, boost::ref(manager), boost::ref(ring_buffer), connect_address, n_io_threads, receive_timeout);
    boost::thread writer_thread(write_h5, boost::ref(manager), boost::ref(format), boost::ref(ring_buffer));

    start_rest_api(manager, rest_port);

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
    if (argc != 6) {
        cout << endl;
        cout << "Usage: h5_zmq_writer [connection_address] [output_file] [n_frames] [rest_port] [user_id]" << endl;
        cout << "\tconnection_address: Address to connect to the stream (PULL). Example: tcp://127.0.0.1:40000" << endl;
        cout << "\toutput_file: Name of the output file." << endl;
        cout << "\tn_frames: Number of images to acquire. 0 for infinity (untill /stop is called)." << endl;
        cout << "\trest_port: Port to start the REST Api on." << endl;
        cout << "\tuser_id: uid under which to run the writer. -1 to leave it as it is." << endl;
        cout << endl;

        exit(-1);
    }

    // This process can be set to run under a different user.
    auto user_id = atoi(argv[5]);
    if (user_id != -1) {

        #ifdef DEBUG_OUTPUT
            cout << "[h5_zmq_writer::main] Setting process uid to " << user_id << endl;
        #endif

        if (setuid(user_id)) {
            stringstream error_message;
            error_message << "[h5_zmq_writer::main] Cannot set user_id to " << user_id << endl;

            throw runtime_error(error_message.str());
        }
    }

    int n_frames =  atoi(argv[3]);
    string output_file = string(argv[2]);

    NXmxFormat format;
    WriterManager manager(format.get_input_value_type(), output_file, n_frames);

    string connect_address = string(argv[1]);
    int rest_port = atoi(argv[4]);

    run_writer(manager, format, connect_address, rest_port);

    return 0;
}
