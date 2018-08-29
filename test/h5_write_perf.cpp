#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>
#include <string>

#include "H5Writer.hpp"

using namespace std;
using namespace std::chrono;

void write_frame(H5Writer& writer, size_t index, char* buffer, size_t buffer_length, 
    char* metadata_buffer, size_t metadata_buffer_length, size_t n_metadata, size_t n_modules) {

    writer.write_data("data",
                      index, 
                      buffer,
                      {n_modules * 512, 1024},
                      buffer_length, 
                      "uint16",
                      "little");

    for (size_t meta_index=0; meta_index < n_metadata; meta_index++) {
        
        writer.write_data(to_string(meta_index),
                          index, 
                          metadata_buffer,
                          {n_modules},
                          metadata_buffer_length, 
                          "uint64",
                          "little");
    }
} 

int main (int argc, char *argv[])
{
    if (argc != 6) {
        cout << endl;
        cout << "Usage: h5_write_perf [output_file] [n_frames] [n_modules] [frame_rate] [n_metadata]" << endl;
        cout << "\toutput_file: Name of the output file." << endl;
        cout << "\tn_frames: Number of images to write." << endl;
        cout << "\tn_modules: Numbers of 512*1024 modules." << endl;
        cout << "\tframe_rate: Frame rate in Hz." << endl;
        cout << "\tn_metadata: Number of metadata datasets to be written." << endl;
        cout << endl;

        exit(-1);
    }

    string output_file = string(argv[1]);
    int n_frames =  atoi(argv[2]);
    int n_modules = atoi(argv[3]);
    int frame_rate = atoi(argv[4]);
    int n_metadata = atoi(argv[5]);

    size_t buffer_length = n_modules * 512 * 1024 * sizeof(u_int16_t);
    char* buffer = new char[buffer_length];
   
    size_t metadata_buffer_length = 4 * n_modules;
    char* metadata_buffer = new char[metadata_buffer_length];

    H5Writer writer(output_file, n_frames, n_frames, n_frames);

    // Initialize all datasets;
    write_frame(writer, 0, buffer, buffer_length, metadata_buffer, metadata_buffer_length, n_metadata, n_modules);

    auto total_sleep_time = 0.0;
    auto total_write_time = 0.0;
    auto missed_frames = 0;
    
    auto start_time_frame = std::chrono::system_clock::now();
    
    for (int index=0; index<n_frames; index++) {
        
        write_frame(writer, index, buffer, buffer_length, metadata_buffer, metadata_buffer_length, n_metadata, n_modules);

        auto time_diff = duration<float, milli>(std::chrono::system_clock::now() - start_time_frame).count();
        auto sleep_time = (1.0/frame_rate*1000) - time_diff;

        if (sleep_time < 0) {
            cout << "Not in time for frame " << index << endl;
            sleep_time = 0;
            missed_frames++;
        }

        usleep(sleep_time*1000);

        total_sleep_time += sleep_time;
        total_write_time += time_diff;

        start_time_frame = std::chrono::system_clock::now();
    }

    writer.close_file();

    cout << "total sleep: " << total_sleep_time/1000 << " total write: " << total_write_time/1000;
    cout << " missed frames: " << missed_frames/float(n_frames)*100 << "%" <<endl;
    
    return 0;
}
