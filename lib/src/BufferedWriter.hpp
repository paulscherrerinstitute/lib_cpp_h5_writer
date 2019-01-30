#ifndef BUFFEREDWRITER_H
#define BUFFEREDWRITER_H

#include "H5Writer.hpp"
#include "MetadataBuffer.hpp"

class BufferedWriter : public H5Writer
{
    size_t total_frames;
    std::unique_ptr<MetadataBuffer> metadata_buffer;

    public:
        BufferedWriter(const std::string& filename, size_t total_frames, std::unique_ptr<MetadataBuffer>&& metadata_buffer, 
            hsize_t frames_per_file=0, hsize_t initial_dataset_size=1000, hsize_t dataset_increase_step=1000);
        virtual void cache_metadata(std::string name, uint64_t frame_index, const char* data);
        virtual void write_metadata_to_file();
};

class DummyBufferedWriter : public BufferedWriter
{
    public:
        DummyBufferedWriter();
        void cache_metadata(std::string name, uint64_t frame_index, const char* data) override;
        void write_metadata_to_file() override;
};

std::unique_ptr<BufferedWriter> get_buffered_writer(const std::string& filename, size_t total_frames, 
    std::unique_ptr<MetadataBuffer> metadata_buffer, hsize_t frames_per_file=0, hsize_t dataset_increase_step=1000);

#endif