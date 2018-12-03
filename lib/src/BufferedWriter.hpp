#ifndef BUFFEREDWRITER_H
#define BUFFEREDWRITER_H

#include "H5Writer.hpp"

class BufferedWriter : public H5Writer
{
    size_t total_frames;

    public:
        BufferedWriter(const std::string& filename, size_t total_frames, hsize_t frames_per_file=0, 
            hsize_t initial_dataset_size=1000, hsize_t dataset_increase_step=1000);
};

class DummyBufferedWriter : public BufferedWriter
{
    public:
        DummyBufferedWriter();
};

std::unique_ptr<BufferedWriter> get_buffered_writer(const std::string& filename, size_t total_frames, 
    hsize_t frames_per_file=0, hsize_t initial_dataset_size=1000, hsize_t dataset_increase_step=1000);

#endif