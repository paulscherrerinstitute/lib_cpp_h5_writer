#ifndef H5CHUNKEDWRITER_H
#define H5CHUNKEDWRITER_H

#include <map>
#include "config.hpp"
#include "h5_utils.hpp"

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
    
    hsize_t prepare_storage_for_frame(size_t frame_index, size_t* frame_shape);
    void create_file(size_t* frame_shape, hsize_t frame_chunk=0);

    public:
        HDF5ChunkedWriter(const std::string filename, const std::string dataset_name, hsize_t frames_per_file=0, hsize_t initial_dataset_size=config::initial_dataset_size);
        ~HDF5ChunkedWriter();
        void close_file();
        void write_data(size_t frame_index, size_t* frame_shape, size_t data_bytes_size, char* data);
        void write_format(h5_group& format_root, std::map<std::string, h5_value>& values);
};

#endif