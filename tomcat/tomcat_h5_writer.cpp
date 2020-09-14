#include <iostream>
#include <sstream>
#include <stdexcept>

#include "config.hpp"
#include "WriterManager.hpp"
#include "ZmqReceiver.hpp"
#include "ZmqSender.hpp"
#include "ProcessManager.hpp"

#include "TomcatFormat.cpp"

int main (int argc, char *argv[])
{
    if (argc != 7) {
        cout << endl;
        cout << "Usage: tomcat_h5_writer [connection_address] [output_file] [n_frames] [user_id]" << endl;
        cout << " [dataset_name] [max_frames_per_file]" << endl;
        cout << "\tconnection_address: Address to connect to the stream (PULL). Example: tcp://127.0.0.1:40000" << endl;
        cout << "\toutput_file: Name of the output file." << endl;
        cout << "\tn_frames: Number of images to acquire. 0 for infinity (until /stop is called)." << endl;
        cout << "\tuser_id: uid under which to run the writer. -1 to leave it as it is." << endl;
        cout << "\tdataset_name: Definition of the dataset name." << endl;
        cout << "\tframes_per_file: Maximum number of frames for each h5 file." << endl;
        cout << endl;
        exit(-1);
    }

    string connect_address = string(argv[1]);
    string output_file = string(argv[2]);
    uint64_t n_frames =  atoi(argv[3]);
    int user_id = atoi(argv[4]);
    string dataset_name = string(argv[5]);
    hsize_t frames_per_file = atoi(argv[6]);
    int n_modules = 1;
    int rest_port = 9555;
    string statistics_monitor_address = "tcp://*:8088";
    // For DEBUG -> http://0.0.0.0:9901
    // string pco_client_rest_address = "http://0.0.0.0:9901";
    // For Release -> http://xbl-daq-32:9901
    string pco_client_rest_address = "http://xbl-daq-32:9901";


    if (user_id != -1) {
        writer_utils::set_process_id(user_id);
    }


    writer_utils::create_destination_folder(output_file);

    auto header_values = shared_ptr<unordered_map<string, HeaderDataType>>(new unordered_map<string, HeaderDataType> {
        {"frame", HeaderDataType("uint64")},
        {"htype", HeaderDataType("uint64", n_modules)},
        {"tag", HeaderDataType("uint64", n_modules)},
        {"source", HeaderDataType("uint64", n_modules)},

        {"shape", HeaderDataType("uint64", 2)},

        {"type", HeaderDataType("uint64", n_modules)},
        {"endianess", HeaderDataType("uint64", n_modules)},
    });

    TomcatFormat format(dataset_name);

    WriterManager writer_manager(format.get_input_value_type(), output_file, dataset_name, user_id, n_frames);
    ZmqReceiver receiver(connect_address, config::zmq_n_io_threads, config::zmq_receive_timeout, header_values);
    ZmqSender sender(statistics_monitor_address, config::zmq_n_io_threads);
    RingBuffer ring_buffer(config::ring_buffer_n_slots);
    uint16_t adjust_n_frames = 1;


    ProcessManager process_manager(writer_manager, sender, receiver, ring_buffer, format, rest_port, pco_client_rest_address, frames_per_file, adjust_n_frames);

    process_manager.run_writer();

    return 0;
}
