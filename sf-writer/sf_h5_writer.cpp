#include <iostream>
#include <stdexcept>
#include <H5Writer.hpp>
#include <BufferUtils.hpp>
#include "BufferMultiReader.hpp"

#include "config.hpp"
#include "SfFormat.cpp"

using namespace std;

int main (int argc, char *argv[])
{
    if (argc != 5) {
        cout << endl;
        cout << "Usage: sf_h5_writer [root_folder]";
        cout << " [output_file] [start_pulse_id] [stop_pulse_id]";
        cout << endl;
        cout << "\troot_folder: Base of the buffer." << endl;
        cout << "\toutput_file: Complete path to the output file." << endl;
        cout << "\tstart_pulse_id: Start pulse_id of retrieval." << endl;
        cout << "\tstop_pulse_id: Stop pulse_id of retrieval." << endl;
        cout << endl;

        exit(-1);
    }

    string root_folder = string(argv[1]);
    string output_file = string(argv[2]);
    uint64_t start_pulse_id = (uint64_t) atoll(argv[3]);
    uint64_t stop_pulse_id = (uint64_t) atoll(argv[4]);

    struct FileBufferMetadata {
        // Needed by RingBuffer
        size_t buffer_slot_index;
        const size_t frame_bytes_size = 2*512*1024*1000;

        uint64_t start_pulse_id;
        uint64_t stop_pulse_id;
        uint16_t module_id;

        uint64_t pulse_id[1000];
        uint64_t frame_id[1000];
        uint32_t daq_rec[1000];
        uint16_t n_received_packets[1000];
    };

    size_t n_modules = 32;

    RingBuffer<FileBufferMetadata> ring_buffer(3);

    auto path_suffixes = BufferUtils::get_path_suffixes(
            start_pulse_id, stop_pulse_id);

    size_t n_reads = path_suffixes.size() * n_modules;

    auto read_buffer = [=, &ring_buffer]() {

        for (size_t i_module=0; i_module<n_modules; i_module++) {
            // TODO: This is ugly. Remove it.
            stringstream name;
            name << "M";
            if (i_module < 10) name << "0";
            name << (int) i_module;
            string device_name = name.str();

            for (const auto& suffix:path_suffixes) {
                auto file_metadata = make_shared<FileBufferMetadata>();
                file_metadata->module_id = i_module;
                file_metadata->start_pulse_id = suffix.start_pulse_id;
                file_metadata->stop_pulse_id = suffix.stop_pulse_id;

                char* buffer = ring_buffer.reserve(file_metadata);
                while (buffer == nullptr) {
                    this_thread::sleep_for(chrono::milliseconds(10));
                    buffer = ring_buffer.reserve(file_metadata);
                }

                string filename =
                        root_folder + "/" +
                        device_name + "/" +
                        suffix.path;

                cout << "Reading file " << filename << endl;

                H5::H5File input_file(filename, H5F_ACC_RDONLY);

                auto image_dataset = input_file.openDataSet("image");
                image_dataset.read(
                        buffer, H5::PredType::NATIVE_UINT16);

                auto pulse_id_dataset = input_file.openDataSet("pulse_id");
                pulse_id_dataset.read(
                        file_metadata->pulse_id, H5::PredType::NATIVE_UINT64);

                auto frame_id_dataset = input_file.openDataSet("frame_id");
                frame_id_dataset.read(
                        file_metadata->frame_id, H5::PredType::NATIVE_UINT64);

                auto daq_rec_dataset = input_file.openDataSet("daq_rec");
                daq_rec_dataset.read(
                        file_metadata->daq_rec, H5::PredType::NATIVE_UINT32);

                auto received_packets_dataset =
                        input_file.openDataSet("received_packets");
                received_packets_dataset.read(
                        file_metadata->n_received_packets,
                        H5::PredType::NATIVE_UINT16);

                input_file.close();

                ring_buffer.commit(file_metadata);
            }
        }
    };
    auto read_thread = thread(read_buffer);

    for (size_t i_read=0; i_read<n_reads; i_read++) {

        auto data = ring_buffer.read();
        while (data.first == nullptr) {
            this_thread::sleep_for(chrono::milliseconds(10));
            data = ring_buffer.read();
        }

        // TODO: Have data, write it down.
        cout << "Got from " << data.first->start_pulse_id;
        cout << " to " << data.first->stop_pulse_id << endl;

    }

    read_thread.join();

    return 0;
}
