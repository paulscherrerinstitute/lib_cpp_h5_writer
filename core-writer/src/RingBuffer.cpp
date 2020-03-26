#include <stdexcept>
#include <iostream>
#include <cstddef>

#include "RingBuffer.hpp"

using namespace std;

RingBuffer::RingBuffer(size_t n_slots) :
        n_slots_(n_slots),
        ringbuffer_slots_(n_slots, 0)
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono; 
        cout << "[" << system_clock::now() << "]";
        cout << "[RingBuffer::RingBuffer] Creating ring buffer";
        cout << " with n_slots " << n_slots << endl;
    #endif
}

RingBuffer::~RingBuffer() 
{
    if (frame_data_buffer_ != NULL) {
        free(frame_data_buffer_);
        frame_data_buffer_ = NULL;
    }
}

void RingBuffer::initialize(size_t slot_size)
{
    if (frame_data_buffer_) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono; 
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[RingBuffer::initialize]";
        err_msg << " Ring buffer already initialized." << endl;

        throw runtime_error(err_msg.str());
    }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono; 
        cout << "[" << system_clock::now() << "]";
        cout << "[RingBuffer::initialize] Initializing ring buffer";
        cout << " with slot_size " << slot_size << endl;
    #endif
    
    this->write_index_ = 0;
    this->slot_size_ = slot_size;
    this->buffer_size_ = slot_size * n_slots_;
    this->frame_data_buffer_ = new char[buffer_size_];
    this->buffer_used_slots_ = 0;
    this->ring_buffer_initialized_ = true;

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono; 
        cout << "[" << system_clock::now() << "]";
        cout << "[RingBuffer::initialize] ";
        cout << " Total buffer_size " << buffer_size_ << endl;
    #endif
}

char* RingBuffer::reserve(shared_ptr<FrameMetadata> frame_metadata)
{
    if (!ring_buffer_initialized_) {
        initialize(frame_metadata->frame_bytes_size);
    }

    if (frame_metadata->frame_bytes_size > slot_size_) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono; 
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[RingBuffer::reserve] Received frame index ";
        err_msg << frame_metadata->frame_index;
        err_msg << " that is too large for ring buffer slot. ";
        err_msg << "Slot size " << slot_size_ << ", but frame bytes size ";
        err_msg << frame_metadata->frame_bytes_size << endl;

        throw runtime_error(err_msg.str());
    }

    // Check and reserve slot in the buffer.
    {
        lock_guard<mutex> lock(ringbuffer_slots_mutex_);

        if (!ringbuffer_slots_[write_index_]) {
            ringbuffer_slots_[write_index_] = 1;
            
            frame_metadata->buffer_slot_index = write_index_;

            #ifdef DEBUG_OUTPUT
                using namespace date;
                using namespace chrono; 
                cout << "[" << system_clock::now() << "]";
                cout << "[RingBuafer::reserve] Ring buffer slot ";
                cout << frame_metadata->buffer_slot_index;
                cout << " reserved for frame_index ";
                cout << frame_metadata->frame_index << endl;
            #endif

            write_index_ = (write_index_ + 1) % n_slots_;
            buffer_used_slots_++;

        } else {
            stringstream err_msg;

            using namespace date;
            using namespace chrono; 
            err_msg << "[" << system_clock::now() << "]";
            err_msg << "[RingBuffer::reserve] Ring buffer is full.";
            err_msg << " Collision at write_index = " << write_index_ << endl;

            throw runtime_error(err_msg.str());
        }
    }

    // The slot is already reserved, no need for synchronization.
    return get_buffer_slot_address(frame_metadata->buffer_slot_index);
}

void RingBuffer::commit(shared_ptr<FrameMetadata> frame_metadata)
{
    lock_guard<mutex> lock(frame_metadata_queue_mutex_);

    frame_metadata_queue_.push_back(frame_metadata);

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[RingBuffer::commit] Metadata for frame_index ";
        cout << frame_metadata->frame_index;
        cout << " added to metadata queue." << endl;
    #endif
}

char* RingBuffer::get_buffer_slot_address(size_t buffer_slot_index)
{
    char* slot_memory_address =
            frame_data_buffer_ + (buffer_slot_index * slot_size_);

    // Check if the memory address is valid.
    if (slot_memory_address > frame_data_buffer_ + buffer_size_) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono; 
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[RingBuffer::get_buffer_slot_address]";
        err_msg << " Ring buffer address out of range." << endl;

        throw runtime_error(err_msg.str());
    }

    return slot_memory_address;
}

pair<shared_ptr<FrameMetadata>, char*> RingBuffer::read()
{
    shared_ptr<FrameMetadata> frame_metadata;

    {
        lock_guard<mutex> lock(frame_metadata_queue_mutex_);

        // A NULL char* means no waiting data in the ring buffer.
        if (frame_metadata_queue_.empty()) {
            return {NULL, NULL};
        }

        frame_metadata = frame_metadata_queue_.front();
        frame_metadata_queue_.pop_front();
    }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono; 
        cout << "[" << system_clock::now() << "]";
        cout << "[RingBuffer::read] Received metadata for frame_index ";
        cout << frame_metadata->frame_index << endl;
    #endif

    // Check if the references ring buffer slot is valid.
    {
        lock_guard<mutex> lock(ringbuffer_slots_mutex_);

        if (!ringbuffer_slots_[frame_metadata->buffer_slot_index]) {
            stringstream err_msg;

            using namespace date;
            using namespace chrono; 
            err_msg << "[" << system_clock::now() << "]";
            err_msg << "[RingBuffer::read] Ring buffer slot";
            err_msg << " referenced in message header ";
            err_msg << frame_metadata->buffer_slot_index << " is empty.";
            err_msg << endl;

            throw runtime_error(err_msg.str());
        }
    }
            
    return {frame_metadata,
            get_buffer_slot_address(frame_metadata->buffer_slot_index)};
}

void RingBuffer::release(size_t buffer_slot_index)
{
    if (buffer_slot_index >= n_slots_) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono; 
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[RingBuffer::release] Slot index ";
        err_msg << buffer_slot_index << " is out of range.";
        err_msg << " Ring buffer n_slots = " << n_slots_ << endl;

        throw runtime_error(err_msg.str());
    }

    {
        lock_guard<mutex> lock(ringbuffer_slots_mutex_);

        if (ringbuffer_slots_[buffer_slot_index]) {
            ringbuffer_slots_[buffer_slot_index] = 0;

            buffer_used_slots_--;

        } else {
            stringstream err_msg;

            using namespace date;
            using namespace chrono; 
            err_msg << "[" << system_clock::now() << "]";
            err_msg << "[RingBuffer::release] Cannot release empty";
            err_msg << " ring buffer slot " << buffer_slot_index << endl;

            throw runtime_error(err_msg.str());
        }
    }
}

bool RingBuffer::is_empty()
{
    lock_guard<mutex> lock(ringbuffer_slots_mutex_);
    
    return buffer_used_slots_ == 0;
}

void RingBuffer::clear()
{
    lock_guard<mutex> lock_slots(ringbuffer_slots_mutex_);
    lock_guard<mutex> lock_metadata(frame_metadata_queue_mutex_);

    write_index_ = 0;
    buffer_used_slots_ = 0;
    ringbuffer_slots_ = vector<bool>(n_slots_, 0);
    frame_metadata_queue_.clear();
}

size_t RingBuffer::get_slot_size()
{
    return slot_size_;
}
