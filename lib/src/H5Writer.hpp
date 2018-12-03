#ifndef H5WRITER_H
#define H5WRITER_H

#include <unordered_map>
#include <memory>
#include <vector>
#include <H5Cpp.h>
#include <chrono>
#include "date.h"

class H5Writer
{
    protected:
        // Initialized in constructor.
        const std::string filename;
        hsize_t frames_per_file;
        hsize_t initial_dataset_size;
        hsize_t dataset_increase_step = 0;

        // State variables.
        hsize_t max_data_index = 0;
        hsize_t current_frame_chunk = 0;

        H5::H5File file;
        std::unordered_map<std::string, H5::DataSet> datasets;
        std::unordered_map<std::string, hsize_t> datasets_current_size;    
        
        hsize_t prepare_storage_for_data(const std::string& dataset_name, const size_t data_index, const std::vector<size_t>& data_shape, 
            const std::string& data_type, const std::string& endianness);

        void create_file(const hsize_t frame_chunk=0);

        void create_dataset(const std::string& dataset_name, const std::vector<size_t>& data_shape, 
            const std::string& data_type, const std::string& endianness);

    public:
        H5Writer(const std::string& filename, hsize_t frames_per_file=0, hsize_t initial_dataset_size=1000, hsize_t dataset_increase_step=1000);
        virtual ~H5Writer();
        virtual bool is_file_open() const;
        virtual void close_file();
        virtual void write_data(const std::string& dataset_name, const size_t data_index, const char* data, const std::vector<size_t>& data_shape, 
            const size_t data_bytes_size, const std::string& data_type, const std::string& endianness);
        virtual H5::H5File& get_h5_file();
        virtual bool is_data_for_current_file(const size_t data_index);
};

class DummyH5Writer : public H5Writer
{
    public:
        DummyH5Writer();
        bool is_file_open() const;
        void close_file();
        void write_data(const std::string& dataset_name, const size_t data_index, const char* data, const std::vector<size_t>& data_shape, 
            const size_t data_bytes_size, const std::string& data_type, const std::string& endianness);
        H5::H5File& get_h5_file();
};

std::unique_ptr<H5Writer> get_h5_writer(const std::string& filename, hsize_t frames_per_file=0, 
    hsize_t initial_dataset_size=1000, hsize_t dataset_increase_step=1000);

#endif