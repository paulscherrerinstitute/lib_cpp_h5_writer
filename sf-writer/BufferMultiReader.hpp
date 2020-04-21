#ifndef BUFFERMULTIREADER_H
#define BUFFERMULTIREADER_H

#include <thread>
#include "RingBuffer.hpp"

class BufferMultiReader
{
    const std::string device_name_;
    const std::string root_folder_;
    std::atomic_bool is_running_;
    uint16_t* frame_buffer_;
    UdpFrameMetadata* frame_metadata_buffer_;

    std::atomic_uint n_modules_left_;
    std::atomic_uint64_t pulse_id_;
    std::vector<std::thread> receiving_threads_;

protected:
    void read_thread(uint8_t module_number);

public:
    BufferMultiReader(
            const std::string& device_name,
            const std::string& root_folder);

    virtual ~BufferMultiReader();

    char* get_buffer();

    UdpFrameMetadata load_frame_to_buffer(const uint64_t pulse_id);
};

#endif