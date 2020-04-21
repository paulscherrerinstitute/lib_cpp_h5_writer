#include <iostream>
#include <stdexcept>
#include <ZmqReceiver.hpp>
#include <ZmqRecvModule.hpp>
#include <H5WriteModule.hpp>
#include <ProcessManager.hpp>
#include <H5Writer.hpp>
#include "BufferMultiReader.hpp"

#include "config.hpp"
#include "SfFormat.cpp"

using namespace std;

int main (int argc, char *argv[])
{
    if (argc != 6) {
        cout << endl;
        cout << "Usage: sf_h5_writer [device_name] [root_folder]";
        cout << " [output_file] [start_pulse_id] [stop_pulse_id]";
        cout << endl;
        cout << "\tdevice_name: Name of detector to write.";
        cout << "\troot_folder: Base of the buffer.";
        cout << "\toutput_file: Complete path to the output file.";
        cout << "\tstart_pulse_id: Start pulse_id of retrieval." << endl;
        cout << "\tstop_pulse_id: Stop pulse_id of retrieval." << endl;
        cout << endl;

        exit(-1);
    }

    string device_name = string(argv[1]);
    string root_folder = string(argv[2]);
    string output_file = string(argv[3]);
    uint64_t start_pulse_id = (uint64_t) atoi(argv[4]);
    uint64_t stop_pulse_id = (uint64_t) atoi(argv[5]);

    BufferMultiReader reader(device_name, root_folder);
    UdpFrameMetadata metadata;
    char* frame_buffer = reader.get_buffer();

    H5Writer writer(output_file);

    for (size_t pulse_id=start_pulse_id;
            pulse_id <= stop_pulse_id;
            pulse_id++) {

        metadata = reader.load_frame_to_buffer(pulse_id);

        writer.write_data("frame", pulse_id,
                          frame_buffer,
                          {32*512, 1024}, 2, "uint16", "little");

        writer.write_data("pulse_id", pulse_id,
                          (char*)&(metadata.pulse_id),
                          {1}, 8, "uint64", "little");

        writer.write_data("frame", pulse_id,
                          (char*)&(metadata.frame_index),
                          {1}, 8, "uint64", "little");

        writer.write_data("daq_rec", pulse_id,
                          (char*)&(metadata.daq_rec),
                          {1}, 8, "uint64", "little");

        uint64_t is_good_frame = 0;
        if (metadata.n_recv_packets == 4096) {
            is_good_frame = 1;
        }

        writer.write_data("is_good_frame", pulse_id,
                          (char*)&(is_good_frame),
                          {1}, 8, "uint64", "little");
    }

    return 0;
}

//unordered_map<string, HeaderDataType> header_values {
//            {"pulse_id", HeaderDataType("uint64")},
//            {"frame", HeaderDataType("uint64")},
//            {"is_good_frame", HeaderDataType("uint64")},
//            {"daq_rec", HeaderDataType("int64")},
//
//            {"pulse_id_diff", HeaderDataType("int64", n_modules)},
//            {"framenum_diff", HeaderDataType("int64", n_modules)},
//
//            {"missing_packets_1", HeaderDataType("uint64", n_modules)},
//            {"missing_packets_2", HeaderDataType("uint64", n_modules)},
//            {"daq_recs", HeaderDataType("uint64", n_modules)},
//
//            {"pulse_ids", HeaderDataType("uint64", n_modules)},
//            {"framenums", HeaderDataType("uint64", n_modules)},
//
//            {"module_number", HeaderDataType("uint64", n_modules)}
//    });