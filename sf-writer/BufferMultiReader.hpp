#ifndef BUFFERMULTIREADER_H
#define BUFFERMULTIREADER_H

#include <thread>
#include "RingBuffer.hpp"

class BufferMultiReader
{
    const std::string device_name;
    const std::string root_folder;

    std::atomic_bool is_running_;
    std::atomic_int n_modules_left_;
    std::atomic_uint64_t pulse_id_;
    std::vector<std::thread> receiving_threads_;

protected:
    void read_thread();

public:
    BufferMultiReader(
            const std::string& device_name,
            const std::string& root_folder);

    virtual ~BufferMultiReader();

    UdpFrameMetadata get_frame(const uint64_t pulse_id, void* frame_buffer);
};

#endif