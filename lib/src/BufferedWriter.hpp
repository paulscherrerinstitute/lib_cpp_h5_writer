#ifndef BUFFEREDWRITER_H
#define BUFFEREDWRITER_H

#include "H5Writer.hpp"
#include "MetadataBuffer.hpp"

class BufferedWriter : public H5Writer
{
    size_t total_frames;
    std::unique_ptr<MetadataBuffer> metadata_buffer;
    uint64_t n_received_frames;

    public:
        BufferedWriter(const std::string& filename, const std::string& dataset_name, size_t total_frames, std::unique_ptr<MetadataBuffer>&& metadata_buffer, 
            hsize_t frames_per_file=0, hsize_t initial_dataset_size=1000, hsize_t dataset_increase_step=1000);
        virtual void cache_metadata(std::string name, uint64_t frame_index, const char* data);
        virtual void write_metadata_to_file(uint64_t n_rec_frames, uint64_t frame_index);
};

class DummyBufferedWriter : public BufferedWriter, public DummyH5Writer
{
    public:
        DummyBufferedWriter() : BufferedWriter("/dev/null", 0, 0, 0, 0) {}
        void cache_metadata(std::string name, uint64_t frame_index, const char* data) override {}
        void write_metadata_to_file(uint64_t n_rec_frames, uint64_t frame_index) override {}

        bool is_file_open() const override 
            { return DummyH5Writer::is_file_open(); }

        void create_file(const hsize_t frame_chunk=1) override 
            { return DummyH5Writer::create_file(frame_chunk); }

        void close_file() override
            { return DummyH5Writer::close_file(); }

        void write_data(const std::string& dataset_name, const size_t data_index, const char* data, const std::vector<size_t>& data_shape, 
            const size_t data_bytes_size, const std::string& data_type, const std::string& endianness) override
        {
            return DummyH5Writer::write_data (
                dataset_name, data_index, data, data_shape, data_bytes_size, data_type, endianness );
        }
        
        H5::H5File& get_h5_file() override
            { return DummyH5Writer::get_h5_file(); }
        
        bool is_data_for_current_file(const size_t data_index) override
            { return DummyH5Writer::is_data_for_current_file(data_index); }
};

std::unique_ptr<BufferedWriter> get_buffered_writer(const std::string& filename, const std::string& dataset_name, size_t total_frames, 
    std::unique_ptr<MetadataBuffer> metadata_buffer, hsize_t frames_per_file=0, hsize_t dataset_increase_step=1000);

#endif