#include <iostream>
#include <sstream>
#include <stdexcept>

#include "config.hpp"
#include "WriterManager.hpp"
#include "ZmqReceiver.hpp"
#include "ProcessManager.hpp"

#include "TomcatFormat.cpp"

int main (int argc, char *argv[])
{
    if (argc != 8) {
        cout << endl;
        cout << "Usage: tomcat_h5_writer [connection_address] [output_file] [n_frames] [user_id] [n_modules] [rest_api_port] [dataset_name]" << endl;
        // cout << " [output_file] [n_frames] [rest_port] [user_id] [n_modules]" << endl;
        cout << "\tconnection_address: Address to connect to the stream (PULL). Example: tcp://127.0.0.1:40000" << endl;
        cout << "\toutput_file: Name of the output file." << endl;
        cout << "\tn_frames: Number of images to acquire. 0 for infinity (until /stop is called)." << endl;
        cout << "\tuser_id: uid under which to run the writer. -1 to leave it as it is." << endl;
        cout << "\tn_modules: Number of detector modules to be written." << endl;
        cout << "\trest_port: Port to start the REST Api on." << endl;
        cout << "\tdataset_name: Definition of the dataset name" << endl;
        cout << endl;
        exit(-1);
    }

    string connect_address = string(argv[1]);
    string output_file = string(argv[2]);
    int n_frames =  atoi(argv[3]);
    int user_id = atoi(argv[4]);
    int n_modules = atoi(argv[5]);
    int rest_port = atoi(argv[6]);
    string dataset_name = string(argv[7]);
    string bsread_rest_address = "http://localhost:9999/";

    if (user_id != -1) {
        writer_utils::set_process_id(user_id);
    }


    writer_utils::create_destination_folder(output_file);   

    auto header_values = shared_ptr<unordered_map<string, HeaderDataType>>(new unordered_map<string, HeaderDataType> {
        {"frame", HeaderDataType("uint64")},
        {"htype", HeaderDataType("uint64", n_modules)},
        {"tag", HeaderDataType("uint64", n_modules)},
        {"source", HeaderDataType("uint64", n_modules)},

        {"shape", HeaderDataType("uint64", n_modules)},

        {"type", HeaderDataType("uint64", n_modules)},
        {"endianess", HeaderDataType("uint64", n_modules)},
    });

    TomcatFormat format(dataset_name);

    WriterManager writer_manager(format.get_input_value_type(), output_file, n_frames);
    ZmqReceiver receiver(connect_address, config::zmq_n_io_threads, config::zmq_receive_timeout, header_values);
    RingBuffer ring_buffer(config::ring_buffer_n_slots);
    uint16_t adjust_n_frames = 1;
    hsize_t frames_per_file = 0;

    ProcessManager process_manager(writer_manager, receiver, ring_buffer, format, rest_port, bsread_rest_address, frames_per_file, adjust_n_frames);
    
    process_manager.run_writer();

    return 0;
}
