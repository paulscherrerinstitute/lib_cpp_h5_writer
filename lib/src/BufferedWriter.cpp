#include <iostream>

#include "H5Format.hpp"
#include "BufferedWriter.hpp"


using namespace std;

BufferedWriter::BufferedWriter(const std::string& filename, const std::string& dataset_name, size_t total_frames, unique_ptr<MetadataBuffer>&& metadata_buffer, 
    hsize_t frames_per_file, hsize_t initial_dataset_size, hsize_t dataset_increase_step) : 
        H5Writer(filename, dataset_name, frames_per_file, initial_dataset_size, dataset_increase_step), 
        total_frames(total_frames), metadata_buffer(move(metadata_buffer))
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[Buffuffered writer"; 
        cout << " with filename " << filename;
        cout << " and total_frames " << total_frames;
        cout << " and frames_per_file " << frames_per_file;
        cout << " and initial_dataset_size " << initial_dataset_size;
        cout << endl;
    #endif
}

void BufferedWriter::cache_metadata(string name, uint64_t frame_index, const char* data)
{
    auto relative_frame_index = get_relative_data_index(frame_index);
    metadata_buffer->add_metadata_to_buffer(name, relative_frame_index, data);
}

void BufferedWriter::write_metadata_to_file(uint64_t n_rec_frames, uint64_t frame_index)
{
    auto relative_frame_index = get_relative_data_index(frame_index);
    auto header_values_type = metadata_buffer->get_header_values_type();
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[BufferedWriter::write_metadata_to_file] Writing metadata to file..." ;
        cout << " dataset_size: " << relative_frame_index << endl;
    #endif

    if (header_values_type) {
        for (const auto& header_type : *header_values_type) {
            auto& dataset_name = header_type.first;
            auto& header_data_type = header_type.second;

            vector<size_t> data_shape = {header_data_type.value_shape};

            // defines the dataset_size based on the relative frame index
            // in case of roll over hdf5 files
            auto dataset_size = relative_frame_index;

            if ((dataset_size == 0) or (dataset_size > frames_per_file)){
                dataset_size = frames_per_file;
            }
            #ifdef DEBUG_OUTPUT
                using namespace date;
                cout << "[" << std::chrono::system_clock::now() << "]";
                cout << "[BufferedWriter::write_metadata_to_file] Dataset_size: " ;
                cout << dataset_size << endl;
            #endif

            create_dataset(dataset_name, data_shape, header_data_type.type, header_data_type.endianness, false, 
                dataset_size);

            H5::AtomType dataset_data_type(H5FormatUtils::get_dataset_data_type(header_data_type.type));
            dataset_data_type.setOrder(H5T_ORDER_LE);
            auto& dataset = datasets.at(dataset_name);
            dataset.write(metadata_buffer->get_metadata_values(dataset_name).get(), dataset_data_type);
        }
    }
}

std::unique_ptr<BufferedWriter> get_buffered_writer(const string& filename, const string& dataset_name, size_t total_frames, 
    std::unique_ptr<MetadataBuffer> metadata_buffer, hsize_t frames_per_file, hsize_t dataset_increase_step)
{
    size_t initial_dataset_size = frames_per_file != 0 ? frames_per_file : total_frames;

    if (filename == "/dev/null") {
        return unique_ptr<BufferedWriter>(new DummyBufferedWriter());
    } else {
        return unique_ptr<BufferedWriter>(new BufferedWriter(filename, dataset_name, total_frames, move(metadata_buffer),
            frames_per_file, initial_dataset_size, dataset_increase_step));
    }
}