#include <iostream>

#include "H5Format.hpp"
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

void BufferedWriter::cache_metadata(string name, uint64_t frame_index, const char* data)
{
    auto relative_frame_index = get_relative_data_index(frame_index);
    metadata_buffer->add_metadata_to_buffer(name, relative_frame_index, data);
}

void BufferedWriter::write_metadata_to_file()
{
    auto header_values_type = metadata_buffer->get_header_values_type();

    if (header_values_type) {
        for (const auto& header_type : *header_values_type) {
            auto& dataset_name = header_type.first;
            auto& header_data_type = header_type.second;

            vector<size_t> data_shape = {header_data_type.value_shape};

            create_dataset(dataset_name, data_shape, header_data_type.type, header_data_type.endianness, false, 
                metadata_buffer->get_n_images());

            H5::AtomType dataset_data_type(H5FormatUtils::get_dataset_data_type(header_data_type.type));
            dataset_data_type.setOrder(H5T_ORDER_LE);

            auto& dataset = datasets.at(dataset_name);
            dataset.write(metadata_buffer->get_metadata_values(dataset_name).get(), dataset_data_type);
        }
    }
}

std::unique_ptr<BufferedWriter> get_buffered_writer(const string& filename, size_t total_frames, 
    std::unique_ptr<MetadataBuffer> metadata_buffer, hsize_t frames_per_file, hsize_t dataset_increase_step)
{
    size_t initial_dataset_size = frames_per_file != 0 ? frames_per_file : total_frames;

    if (filename == "/dev/null") {
        return unique_ptr<BufferedWriter>(new DummyBufferedWriter());
    } else {
        return unique_ptr<BufferedWriter>(new BufferedWriter(filename, total_frames, move(metadata_buffer),
            frames_per_file, initial_dataset_size, dataset_increase_step));
    }
}