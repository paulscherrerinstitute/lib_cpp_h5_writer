#include <iostream>
#include <chrono>
#include <UdpReceiver.hpp>
#include <fstream>
#include "jungfrau.hpp"


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

    while (true) {
        std::ifstream latest_input_file;
        latest_input_file.open(current_filename);

        std::stringstream strStream;
        strStream << latest_input_file.rdbuf();
        std::string filename = strStream.str();

        std::cout << "Opening " << filename << endl;

        H5::H5File input_file(filename, H5F_ACC_RDONLY |  H5F_ACC_SWMR_READ);
        auto image_dataset = input_file.openDataSet("image");
        auto pulse_id_dataset = input_file.openDataSet("pulse_id");

        ::memset(pulse_id_buffer, 0, sizeof(pulse_id_buffer));

        while (true) {

            pulse_id_dataset.read(
                    pulse_id_buffer,
                    H5::PredType::NATIVE_UINT64);

            uint64_t last_pulse_id = 0;
            for (size_t i=0; i<1000; i++) {
                if (pulse_id_buffer[i] > last_pulse_id) {
                    last_pulse_id = pulse_id_buffer[i];
                }
            }

            cout << last_pulse_id << endl;

            if (pulse_id_buffer[999] != 0) {
                break;
            }
        }

    }
}
