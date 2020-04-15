#include <iostream>

#include "H5Format.hpp"
#include "BufferedWriter.hpp"


using namespace std;

BufferedWriter::BufferedWriter(
        const std::string& filename,
        size_t total_frames,
        MetadataBuffer& metadata_buffer,
        hsize_t frames_per_file,
        hsize_t initial_dataset_size,
        hsize_t dataset_increase_step) :
            H5Writer(
                    filename,
                    frames_per_file,
                    initial_dataset_size,
                    dataset_increase_step),
            total_frames(total_frames),
            metadata_buffer(metadata_buffer)
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

void BufferedWriter::cache_metadata(
        const string& name,
        const uint64_t frame_index,
        const char* data)
{
    auto relative_frame_index =
            get_relative_data_index(static_cast<const size_t>(frame_index));

    metadata_buffer.add_metadata_to_buffer(name, relative_frame_index, data);
}

void BufferedWriter::write_metadata_to_file()
{
    auto header_values_type = metadata_buffer.get_header_values_type();

    if (!header_values_type.empty()) {
        for (const auto &header_type : header_values_type) {
            auto &dataset_name = header_type.first;
            auto &header_data_type = header_type.second;

            vector<size_t> data_shape = {header_data_type.value_shape};

            create_dataset(
                    dataset_name,
                    data_shape,
                    header_data_type.type,
                    header_data_type.endianness,
                    false,
                    metadata_buffer.get_n_slots());

            H5::AtomType dataset_data_type(
                    H5FormatUtils::get_dataset_data_type(
                            header_data_type.type));
            dataset_data_type.setOrder(H5T_ORDER_LE);

            auto &dataset = datasets.at(dataset_name);
            dataset.write(
                    metadata_buffer.get_metadata_values(dataset_name).get(),
                    dataset_data_type);
        }
    }
}