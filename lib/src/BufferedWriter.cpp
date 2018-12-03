#include "BufferedWriter.hpp"

using namespace std;

BufferedWriter::BufferedWriter(const std::string& filename, size_t total_frames, unique_ptr<MetadataBuffer>&& metadata_buffer, 
    hsize_t frames_per_file, hsize_t initial_dataset_size, hsize_t dataset_increase_step) : 
        H5Writer(filename, frames_per_file, initial_dataset_size, dataset_increase_step), 
        total_frames(total_frames), metadata_buffer(move(metadata_buffer))
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[BufferedWriter::BufferedWriter] Creating buffered writer"; 
        cout << " with filename " << filename;
        cout << " and total_frames " << total_frames;
        cout << " and frames_per_file " << frames_per_file;
        cout << " and initial_dataset_size " << initial_dataset_size;
        cout << endl;
    #endif
}

DummyBufferedWriter::DummyBufferedWriter() : BufferedWriter("/dev/null", 0, 0, 0, 0){}

std::unique_ptr<BufferedWriter> get_buffered_writer(const string& filename, size_t total_frames, 
    std::unique_ptr<MetadataBuffer> metadata_buffer, hsize_t frames_per_file, 
    hsize_t initial_dataset_size, hsize_t dataset_increase_step)
{
    if (filename == "/dev/null") {
        return unique_ptr<BufferedWriter>(new DummyBufferedWriter());
    } else {
        return unique_ptr<BufferedWriter>(new BufferedWriter(filename, total_frames, move(metadata_buffer),
            frames_per_file, initial_dataset_size, dataset_increase_step));
    }
}