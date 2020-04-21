#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <list>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <memory>
#include <string>
#include <boost/any.hpp>
#include <chrono>
#include "date.h"

#pragma pack(push)
#pragma pack(1)
struct FileBufferMetadata {
    // Needed by RingBuffer
    const uint64_t frame_bytes_size = 2*512*1024*1000;
    uint64_t buffer_slot_index;

    uint64_t start_pulse_id;
    uint64_t stop_pulse_id;
    uint16_t module_id;

    uint64_t pulse_id[1000];
    uint64_t frame_index[1000];
    uint32_t daq_rec[1000];
    uint16_t n_received_packets[1000];
};
#pragma pack(pop)

struct FrameMetadata
{
    // Ring buffer needed data.
    size_t buffer_slot_index;
    size_t frame_bytes_size;
    
    // Image header data.
    uint64_t frame_index;
    std::string endianness;
    std::string type;
    std::vector<size_t> frame_shape;

    // Pass additional header values.
    std::map<std::string, std::shared_ptr<char>> header_values;
};

struct UdpFrameMetadata
{
    // Ring buffer needed data.
    size_t buffer_slot_index;
    size_t frame_bytes_size;

    uint64_t pulse_id;
    uint64_t frame_index;
    uint32_t daq_rec;
    uint16_t n_recv_packets;
};

template <class T>
class RingBuffer
{
    // Initialized in constructor.
    size_t n_slots_ = 0;
    std::vector<bool> ringbuffer_slots_;

    // Set in initialize().
    size_t slot_size_ = 0;
    size_t buffer_size_ = 0;
    char* frame_data_buffer_ = NULL;
    size_t write_index_ = 0;
    size_t buffer_used_slots_ = 0;
    std::atomic_bool initialized_ = false;

    std::list< std::shared_ptr<T> > frame_metadata_queue_;
    std::mutex frame_metadata_queue_mutex_;
    std::mutex ringbuffer_slots_mutex_;

    char* get_buffer_slot_address(size_t buffer_slot_index);

    public:
        RingBuffer(size_t n_slots);

        virtual ~RingBuffer();
        void initialize(size_t slot_size);
        
        char* reserve(std::shared_ptr<T> metadata);
        void commit(std::shared_ptr<T> metadata);
        std::pair<std::shared_ptr<T>, char*> read();
        void release(size_t buffer_slot_index);

        bool is_empty();
        bool is_initialized();
        void clear();
        size_t get_slot_size();
};

#endif
