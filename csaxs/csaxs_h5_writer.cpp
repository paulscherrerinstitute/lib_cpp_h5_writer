#include <iostream>
#include <sstream>
#include <stdexcept>

#include "config.hpp"
#include "WriterManager.hpp"
#include "ZmqReceiver.hpp"
#include "ProcessManager.hpp"

#include "CsaxsFormat.cpp"

int main (int argc, char *argv[])
{
    if (argc != 7) {
        cout << endl;
        cout << "Usage: csaxs_h5_writer [connection_address] [output_file] [n_frames]";
        cout << " [rest_port] [user_id] [n_modules]" << endl;
        cout << "\tconnection_address: Address to connect to the stream (PULL). Example: tcp://127.0.0.1:40000" << endl;
        cout << "\toutput_file: Name of the output file." << endl;
        cout << "\tn_frames: Number of images to acquire. 0 for infinity (until /stop is called)." << endl;
        cout << "\trest_port: Port to start the REST Api on." << endl;
        cout << "\tuser_id: uid under which to run the writer. -1 to leave it as it is." << endl;
        cout << "\tn_modules: Number of detector modules to be written." << endl;
        cout << endl;

        exit(-1);
    }

    string connect_address = string(argv[1]);
    string output_file = string(argv[2]);
    int n_frames =  atoi(argv[3]);
    int rest_port = atoi(argv[4]);
    int user_id = atoi(argv[5]);
    int n_modules = atoi(argv[6]);
    string bsread_rest_address = "http://localhost:9999/";

    if (user_id != -1) {
        writer_utils::set_process_id(user_id);
    }

    writer_utils::create_destination_folder(output_file);   

    auto header_values = shared_ptr<unordered_map<string, HeaderDataType>>(new unordered_map<string, HeaderDataType> {
        {"pulse_id", HeaderDataType("uint64")},
        {"frame", HeaderDataType("uint64")},
        {"is_good_frame", HeaderDataType("uint64")},
        {"daq_rec", HeaderDataType("int64")},

        {"pulse_id_diff", HeaderDataType("int64", n_modules)},
        {"framenum_diff", HeaderDataType("int64", n_modules)},

        {"missing_packets_1", HeaderDataType("uint64", n_modules)},
        {"missing_packets_2", HeaderDataType("uint64", n_modules)},
        {"daq_recs", HeaderDataType("uint64", n_modules)},
        
        {"pulse_ids", HeaderDataType("uint64", n_modules)},
        {"framenums", HeaderDataType("uint64", n_modules)},
        
        {"module_number", HeaderDataType("uint64", n_modules)}
    });

    CsaxsFormat format("images");

    WriterManager writer_manager(format.get_input_value_type(), output_file, n_frames);
    ZmqReceiver receiver(connect_address, config::zmq_n_io_threads, config::zmq_receive_timeout, header_values);
    RingBuffer ring_buffer(config::ring_buffer_n_slots);

    ProcessManager process_manager(writer_manager, receiver, ring_buffer, format, rest_port, bsread_rest_address);
    process_manager.run_writer();

    return 0;
}
