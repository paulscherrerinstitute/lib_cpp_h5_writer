#include <iostream>
#include <sstream>
#include <stdexcept>

#include "H5Writer.hpp"

using namespace std;

int main (int argc, char *argv[])
{
    if (argc != 5) {
        cout << endl;
        cout << "Usage: h5_write_perf [output_file] [n_frames] [frame_size_x] [frame_size_y]" << endl;
        cout << "\toutput_file: Name of the output file." << endl;
        cout << "\tn_frames: Number of images to write." << endl;
        cout << "\frame_size_x: Frame width in pixels." << endl;
        cout << "\frame_size_y: Frame height in pixels." << endl;
        cout << endl;

        exit(-1);
    }

    string output_file = string(argv[1]);
    int n_frames =  atoi(argv[2]);
    int frame_size_x = atoi(argv[3]);
    int frame_size_y = atoi(argv[4]);

    H5Writer writer(output_file, n_frames, n_frames, n_frames);

    size_t buffer_length = frame_size_x * frame_size_y * sizeof(u_int16_t);
    char* buffer = new char[buffer_length];

    string dataset_name = "data";

    for (size_t index=0; index<n_frames; index++) {
        writer.write_data(dataset_name,
                          index, 
                          buffer,
                          {frame_size_y, frame_size_x},
                          buffer_length, 
                          "uint16",
                          "little");
    }

    writer.close_file();

    

    return 0;
}
