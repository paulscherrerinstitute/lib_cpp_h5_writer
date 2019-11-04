#include <stdexcept>
#include <sstream>
#include <cstring>
#include <iostream>
#include <cstddef>

#include "RingBuffer.hpp"

using namespace std;

RingBuffer::RingBuffer(size_t n_slots) : n_slots(n_slots), ringbuffer_slots(n_slots, 0)
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[RingBuffer::RingBuffer] Creating ring buffer with n_slots " << n_slots << endl;
    #endif
}

RingBuffer::~RingBuffer() 
{
    // If the frame buffer is allocated, free it.
    if (frame_data_buffer != NULL) {
        free(frame_data_buffer);
        frame_data_buffer = NULL;
    }
}

void RingBuffer::initialize(size_t slot_size)
{
    // Check if the ring buffer is already initialized.
    if (frame_data_buffer) {
        stringstream error_message;
        using namespace date;
        error_message << "[" << std::chrono::system_clock::now() << "]";
        error_message << "[RingBuffer::initialize] Ring buffer already initialized." << endl;

        throw runtime_error(error_message.str());
    }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]"; 
        cout << "[RingBuffer::initialize] Initializing ring buffer with slot_size " << slot_size << endl;
    #endif
    
    this->write_index = 0;
    this->slot_size = slot_size;
    this->buffer_size = slot_size * n_slots;
    this->frame_data_buffer = new char[buffer_size];
    this->buffer_used_slots = 0;
    this->ring_buffer_initialized = true;

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[RingBuffer::initialize] Total buffer_size " << buffer_size << endl;
    #endif
}

void RingBuffer::write(shared_ptr<FrameMetadata> frame_metadata, const char* data)
{
    // Initialize the buffer on the first write.
    if (!ring_buffer_initialized) {
        initialize(frame_metadata->frame_bytes_size);
    }

    // All images must fit in the ring buffer.
    if (frame_metadata->frame_bytes_size > slot_size) {
        stringstream error_message;
        using namespace date;
        error_message << "[" << std::chrono::system_clock::now() << "]";
        error_message << "[RingBuffer::write] Received frame index "<< frame_metadata->frame_index;
        error_message << " that is too large for ring buffer slot. ";
        error_message << "Slot size " << slot_size << ", but frame bytes size " << frame_metadata->frame_bytes_size << endl;

        throw runtime_error(error_message.str());
    }

    // Check and reserve slot in the buffer.
    {
        lock_guard<mutex> lock(ringbuffer_slots_mutex);

        if (!ringbuffer_slots[write_index]) {
            ringbuffer_slots[write_index] = 1;
            
            // Set the write index in the FrameMetadata object.
            frame_metadata->buffer_slot_index = write_index;

            #ifdef DEBUG_OUTPUT
                using namespace date;
                cout << "[" << std::chrono::system_clock::now() << "]";
                cout << "[RingBuffer::write] Ring buffer slot " << frame_metadata->buffer_slot_index << " reserved for frame_index ";
                cout << frame_metadata->frame_index << endl;
            #endif

            // Increase and wrap the write index around if needed.
            write_index = (write_index + 1) % n_slots;

            // Keep track of the number of used slots.
            buffer_used_slots++;
        } else {
            stringstream error_message;
            using namespace date;
            error_message << "[" << std::chrono::system_clock::now() << "]";
            error_message << "[RingBuffer::write] Ring buffer is full. Collision at write_index = " << write_index << endl;

            throw runtime_error(error_message.str());
        }
    }

    // The slot is already reserved, no need for synchronization.
    char* slot_memory_address = get_buffer_slot_address(frame_metadata->buffer_slot_index);
    memcpy(slot_memory_address, data, frame_metadata->frame_bytes_size);

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[RingBuffer::write] Copied " << frame_metadata->frame_bytes_size << " frame bytes to buffer_slot_index ";
        cout << frame_metadata->buffer_slot_index << endl;
    #endif

    // Add metadata header to the inter-thread communication queue.
    {
        lock_guard<mutex> lock(frame_metadata_queue_mutex);

        frame_metadata_queue.push_back(frame_metadata);
    }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[RingBuffer::write] Metadata for frame_index " << frame_metadata->frame_index << " added to metadata queue." << endl;
    #endif
}

char* RingBuffer::get_buffer_slot_address(size_t buffer_slot_index)
{
    char* slot_memory_address = frame_data_buffer + (buffer_slot_index * slot_size);

    // Check if the memory address is valid.
    if (slot_memory_address > frame_data_buffer + buffer_size) {
        stringstream error_message;
        using namespace date;
        error_message << "[" << std::chrono::system_clock::now() << "]";
        error_message << "[RingBuffer::get_buffer_slot_address] Calculated ring buffer address is out of bound for buffer_slot_index ";
        error_message << buffer_slot_index << endl;

        throw runtime_error(error_message.str());
    }

    return slot_memory_address;
}

pair<shared_ptr<FrameMetadata>, char*> RingBuffer::read()
{
    shared_ptr<FrameMetadata> frame_metadata;

    // Read data from the metadata queue.
    {
        lock_guard<mutex> lock(frame_metadata_queue_mutex);

        // A NULL char* indicates that there are no available data in the ring buffer.
        if (frame_metadata_queue.empty()) {
            return {NULL, NULL};
        }

        frame_metadata = frame_metadata_queue.front();
        frame_metadata_queue.pop_front();
    }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[RingBuffer::read] Received metadata for frame_index " << frame_metadata->frame_index << endl;
    #endif

    // Check if the references ring buffer slot is valid.
    {
        lock_guard<mutex> lock(ringbuffer_slots_mutex);

        if (!ringbuffer_slots[frame_metadata->buffer_slot_index]) {
            stringstream error_message;
            using namespace date;
            error_message << "[" << std::chrono::system_clock::now() << "]";
            error_message << "[RingBuffer::read] Ring buffer slot referenced in message header ";
            error_message << frame_metadata->buffer_slot_index << " is empty." << endl;

            throw runtime_error(error_message.str());
        }
    }

    char* slot_memory_address = get_buffer_slot_address(frame_metadata->buffer_slot_index);
            
    return {frame_metadata, slot_memory_address};
}

void RingBuffer::release(size_t buffer_slot_index)
{
    // Cannot release a slot index that is out of range.
    if (buffer_slot_index >= n_slots) {
        stringstream error_message;
        using namespace date;
        error_message << "[" << std::chrono::system_clock::now() << "]";
        error_message << "[RingBuffer::release] Slot index to release " << buffer_slot_index;
        error_message << " is out of range. Ring buffer n_slots = " << n_slots << endl;

        throw runtime_error(error_message.str());
    }

    // Release the buffer slot.
    {
        lock_guard<mutex> lock(ringbuffer_slots_mutex);

        if (ringbuffer_slots[buffer_slot_index]) {
            ringbuffer_slots[buffer_slot_index] = 0;

            // Keep track of the number of used slots.
            buffer_used_slots--;

        } else {
            stringstream error_message;
            using namespace date;
            error_message << "[" << std::chrono::system_clock::now() << "]";
            error_message << "[RingBuffer::release] Cannot release empty ring buffer slot " << buffer_slot_index << endl;

            throw runtime_error(error_message.str());
        }
    }
}

size_t RingBuffer::free_slots()
{
    lock_guard<mutex> lock(ringbuffer_slots_mutex);
    return buffer_size - buffer_used_slots;
}

bool RingBuffer::is_empty()
{
    lock_guard<mutex> lock(ringbuffer_slots_mutex);
    return buffer_used_slots == 0;
}

bool RingBuffer::is_initialized()
{
    return ring_buffer_initialized;
}