#ifndef H5CHUNKEDWRITER_H
#define H5CHUNKEDWRITER_H

#include <H5Cpp.h>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace config
{
    auto dataset_type = H5::PredType::NATIVE_UINT8;
    auto dataset_byte_order = H5T_ORDER_LE;
    hsize_t dataset_increase_step = 1000;
    hsize_t initial_dataset_size = 1000;
}

struct MessageMetadata
{
    hsize_t buffer_index = 0;
    uint32_t size = 0;

    hsize_t frame_index = 0;
    hsize_t frame_shape[2];
    std::string dtype;
};

hsize_t expand_dataset(const H5::DataSet& dataset, hsize_t frame_index, hsize_t dataset_increase_step);

void compact_dataset(const H5::DataSet& dataset, hsize_t max_frame_index);

class HDF5ChunkedWriter
{
    // Initialized in constructor.
    std::string filename;
    std::string dataset_name;
    hsize_t frames_per_file;
    hsize_t initial_dataset_size;

    // Configuration parameters.
    hsize_t dataset_increase_step = config::dataset_increase_step;
    
    // State variables.
    hsize_t max_frame_index = 0;
    hsize_t current_dataset_size = 0;
    hsize_t current_frame_chunk = 0;

    H5::H5File file;
    H5::DataSet dataset;
    
    hsize_t prepare_storage_for_frame(const MessageMetadata& metadata);
    void create_file(const MessageMetadata& metadata, hsize_t frame_chunk=0);

    public:
        HDF5ChunkedWriter();
        HDF5ChunkedWriter(const std::string filename, const std::string dataset_name, hsize_t frames_per_file=0, hsize_t initial_dataset_size=config::initial_dataset_size);
        void close_file();
        void write_data(const MessageMetadata& metadata, char* data);
};

#endif