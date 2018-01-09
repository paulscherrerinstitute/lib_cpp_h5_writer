#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <cstddef>
#include <list>
#include <thread>
#include <vector>
#include <map>
#include <stdexcept>
#include <mutex>
#include <sstream>
#include <cstring>
#include <iostream>

struct FrameMetadata
{
    // Needed for the buffer.
    size_t buffer_slot_index = 0;
    size_t frame_bytes_size = 0;

    // Part of the message header.
    size_t frame_index = 0;
    size_t frame_shape[2];
};

class RingBuffer
{
    // Initialized in constructor.
    size_t n_slots;
    std::vector<bool> ringbuffer_slots;    

    // Set in initialize().
    size_t slot_size = 0;
    size_t buffer_size = 0;
    char* frame_data_buffer = NULL;
    size_t write_index = 0;
    size_t buffer_used_slots;

    std::list<FrameMetadata> frame_metadata_queue;
    std::mutex frame_metadata_queue_mutex;
    std::mutex ringbuffer_slots_mutex;

    char* get_buffer_slot_address(size_t buffer_slot_index);

    public:
        RingBuffer(size_t n_slots);
        ~RingBuffer();
        void initialize(size_t slot_size);
        void write(FrameMetadata &metadata, char* data);
        std::pair<FrameMetadata, char*>  read();
        void release(size_t buffer_slot_index);
        bool is_empty();
        
};

#endif