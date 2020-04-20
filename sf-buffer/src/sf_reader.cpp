#include <iostream>
#include <chrono>
#include <UdpReceiver.hpp>
#include <fstream>
#include <thread>
#include "jungfrau.hpp"
#include "BufferUtils.hpp"


using namespace std;

int main (int argc, char *argv[]) {
    if (argc != 3) {
        cout << endl;
        cout << "Usage: sf_reader [device_name] [root_folder]";
        cout << endl;
        cout << "\tdevice_name: Device files to read.";
        cout << "\troot_folder: FS root folder." << endl;
        cout << endl;

        exit(-1);
    }

    string device_name = string(argv[1]);
    string root_folder = string(argv[2]);

    string current_filename = root_folder + "/" + device_name + "/CURRENT";

    uint64_t pulse_id_buffer[1000];
    string last_open_file = "";
    uint64_t last_pulse_id = 0;

    while (true) {
        auto filename = BufferUtils::get_latest_file(current_filename);

        if (last_open_file == filename) {
            this_thread::sleep_for(chrono::milliseconds(100));
            cout << "Waiting for CURRENT to change." << endl;
            continue;
        }

        std::cout << "Opening " << filename << endl;
        last_open_file = filename;
        last_pulse_id = 0;

        H5::H5File input_file(filename, H5F_ACC_RDONLY |  H5F_ACC_SWMR_READ);
        auto image_dataset = input_file.openDataSet("image");
        auto pulse_id_dataset = input_file.openDataSet("pulse_id");

        ::memset(pulse_id_buffer, 0, sizeof(pulse_id_buffer));

        while (true) {
            this_thread::sleep_for(chrono::milliseconds(100));
            H5Drefresh(pulse_id_dataset.getId());

            pulse_id_dataset.read(
                    pulse_id_buffer,
                    H5::PredType::NATIVE_UINT64);

            auto file_frame_index =
                    BufferUtils::get_file_frame_index(last_pulse_id);

            size_t n_new_pulses = 0;
            for (size_t i=file_frame_index; i<1000; i++) {
                if (pulse_id_buffer[i] > last_pulse_id) {
                    last_pulse_id = pulse_id_buffer[i];
                    n_new_pulses++;
                }
            }

            if (n_new_pulses > 0) {
                cout << last_pulse_id << " and n_new_pulses ";
                cout << n_new_pulses << endl;
            }

            if (pulse_id_buffer[999] != 0) {
                break;
            }
        }

    }
}
