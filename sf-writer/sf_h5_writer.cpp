#include <iostream>
#include <stdexcept>
#include <ZmqReceiver.hpp>
#include <ZmqRecvModule.hpp>
#include <H5WriteModule.hpp>
#include <ProcessManager.hpp>

#include "config.hpp"
#include "SfFormat.cpp"

using namespace std;

int main (int argc, char *argv[])
{
    if (argc != 6) {
        cout << endl;
        cout << "Usage: sf_h5_writer [connection_address] [rest_port]";
        cout << " [bsread_address] [n_modules] [n_bad_modules] [detector_name]";
        cout << endl;
        cout << "\tconnection_address: Address to connect to the stream";
        cout << " (PULL). Example: tcp://127.0.0.1:40000" << endl;
        cout << "\trest_port: Port to start the REST Api on." << endl;
        cout << "\tn_modules: Number of detector modules to be written." << endl;
        cout << "\tn_bad_modules: Number of detector modules which";
        cout << " has more then half bad pixels" << endl;
        cout << "\tdetector_name: Name of the detector,";
        cout << " data will be written as data/detector_name/" << endl;
        cout << endl;

        exit(-1);
    }

    string connect_address = string(argv[1]);
    int rest_port = atoi(argv[2]);
    int n_modules = atoi(argv[3]);
    int n_bad_modules = atoi(argv[4]);
    string detector_name = string(argv[5]);

    unordered_map<string, HeaderDataType> header_values {
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
    };

    SfFormat format(detector_name, n_bad_modules);
    RingBuffer ring_buffer(config::ring_buffer_n_slots);

    ZmqRecvModule recv_module(ring_buffer, header_values);
    H5WriteModule write_module(ring_buffer, header_values, format);

    recv_module.start_recv(connect_address, 4);

    ProcessManager process_manager(write_module, recv_module);
    process_manager.start_rest_api(rest_port);

    return 0;
}
