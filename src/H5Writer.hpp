#ifndef H5WRITER_H
#define H5WRITER_H

#include <map>
#include <H5Cpp.h>

class H5Writer
{
    // Initialized in constructor.
    std::string filename;
    std::string dataset_name;
    hsize_t frames_per_file;
    hsize_t initial_dataset_size;
    hsize_t dataset_increase_step = 0;

    // State variables.
    hsize_t max_frame_index = 0;
    hsize_t current_dataset_size = 0;
    hsize_t current_frame_chunk = 0;

    H5::H5File file;
    H5::DataSet dataset;
    
    hsize_t prepare_storage_for_frame(size_t frame_index, size_t* frame_shape, std::string& data_type, std::string& endianness);
    void create_file(size_t* frame_shape, hsize_t frame_chunk, std::string& data_type, std::string& endianness);

    public:
        H5Writer(const std::string filename, const std::string dataset_name, 
            hsize_t frames_per_file=0, hsize_t initial_dataset_size=1000, hsize_t dataset_increase_step=1000);
        ~H5Writer();
        bool is_file_open();
        void close_file();
        void write_frame_data(size_t frame_index, size_t* frame_shape, size_t data_bytes_size, 
            char* data, std::string data_type, std::string endianness);
        H5::H5File& get_h5_file();
};

#endif