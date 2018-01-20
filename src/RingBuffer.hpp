#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <list>
#include <vector>
#include <map>
#include <mutex>
#include <string>


struct FrameMetadata
{
    FrameMetadata(){}

    FrameMetadata(const FrameMetadata& other) : 
        buffer_slot_index(other.buffer_slot_index), frame_bytes_size(other.frame_bytes_size), frame_index(other.frame_index), 
        endianness(other.endianness), type(other.type) {
        frame_shape[0] = other.frame_shape[0];
        frame_shape[1] = other.frame_shape[1];
    }

    // Ring buffer needed data.
    size_t buffer_slot_index = -1;
    size_t frame_bytes_size = 0;
    
    // Image header data.
    uint64_t frame_index = -1;
    std::string endianness = "";
    std::string type;
    size_t frame_shape[2];
};

class RingBuffer
{
    // Initialized in constructor.
    size_t n_slots = 0;
    std::vector<bool> ringbuffer_slots;    

    // Set in initialize().
    size_t slot_size = 0;
    size_t buffer_size = 0;
    char* frame_data_buffer = NULL;
    size_t write_index = 0;
    size_t buffer_used_slots = 0;
    bool ring_buffer_initialized = false;

    std::list<FrameMetadata> frame_metadata_queue;
    std::mutex frame_metadata_queue_mutex;
    std::mutex ringbuffer_slots_mutex;

    char* get_buffer_slot_address(size_t buffer_slot_index);

    public:
        RingBuffer(size_t n_slots);
        ~RingBuffer();
        void initialize(size_t slot_size);
        void write(FrameMetadata &metadata, char* data);
        std::pair<FrameMetadata, char*> read();
        void release(size_t buffer_slot_index);
        bool is_empty();
        
};

#endif