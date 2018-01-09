#include "RingBuffer.hpp"

RingBuffer::RingBuffer(size_t n_slots) : n_slots(n_slots), ringbuffer_slots(n_slots, 0){
    #ifdef DEBUG
        std::cout << "[RingBuffer] Creating ring buffer with n_slots " << n_slots << std::endl;
    #endif
}

RingBuffer::~RingBuffer() {
    // If the frame buffer is allocated, free it.
    if (frame_data_buffer != NULL) {
        free(frame_data_buffer);
        frame_data_buffer = NULL;
    }
}

void RingBuffer::initialize(size_t slot_size)
{
    // Check if the ring buffer is already initialized.
    if (frame_data_buffer != NULL) {
        std::stringstream error_message;
        error_message << "Ring buffer already initialized." << std::endl;

        throw std::runtime_error(error_message.str());
    }

    #ifdef DEBUG
        std::cout << "[RingBuffer] Initializing ring buffer with slot_size " << slot_size << std::endl;
    #endif
    
    this->write_index = 0;
    this->slot_size = slot_size;
    this->buffer_size = slot_size * n_slots;
    this->frame_data_buffer = new char[buffer_size];
}

void RingBuffer::write(FrameMetadata &metadata, char* data)
{
    // All images must fit in the ring buffer.
    if (metadata.frame_bytes_size > slot_size) {
        std::stringstream error_message;
        error_message << "Received frame index "<< metadata.frame_index <<" that is too large for ring buffer slot." << std::endl;
        error_message << "RingBuffer slot size " << slot_size << ", but frame bytes size " << metadata.frame_bytes_size << std::endl;

        throw std::runtime_error(error_message.str());
    }

    // Check and reserve slot in the buffer.
    ringbuffer_slots_mutex.lock();

    if (!ringbuffer_slots[write_index]) {
        ringbuffer_slots[write_index] = 1;
        
        // Set the write index in the FrameMetadata object.
        metadata.buffer_slot_index = write_index;

        #ifdef DEBUG
            std::cout << "[RingBuffer] Ring buffer slot " << metadata.buffer_slot_index << " reserved for frame_index " << metadata.frame_index << std::endl;
        #endif

        // Increase and wrap the write index around if needed.
        write_index = (write_index + 1) % n_slots;

    } else {
        std::stringstream error_message;
        error_message << "Ring buffer is full. Collision at write_index = " << write_index << std::endl;

        throw std::runtime_error(error_message.str());
    }

    ringbuffer_slots_mutex.unlock();

    // Write to the buffer. The slot is already reserved.
    char* slot_memory_address = get_buffer_slot_address(metadata.buffer_slot_index);
    std::memcpy(slot_memory_address, data, metadata.frame_bytes_size);

    #ifdef DEBUG
        std::cout << "[RingBuffer] Copied " << metadata.frame_bytes_size << " frame bytes to buffer_slot_index " << metadata.buffer_slot_index << std::endl;
    #endif
    
    frame_metadata_queue_mutex.lock();

    // Send the metadata header to writing process.
    frame_metadata_queue.push_back(metadata);

    frame_metadata_queue_mutex.unlock();

    #ifdef DEBUG
        std::cout << "[RingBuffer] Metadata for frame_index " << metadata.frame_index << " added to metadata queue." << std::endl;
    #endif
}

char* RingBuffer::get_buffer_slot_address(size_t buffer_slot_index) {
    char* slot_memory_address = frame_data_buffer + (buffer_slot_index * slot_size);

    // Check if the memory address is valid.
    if (slot_memory_address > frame_data_buffer + buffer_size) {
        std::stringstream error_message;
        error_message << "Calculated ring buffer address is out of bound for buffer_slot_index " << buffer_slot_index << std::endl;

        throw std::runtime_error(error_message.str());
    }

    #ifdef DEBUG
        std::cout << "[RingBuffer] For buffer_slot_index " << buffer_slot_index << " the calculated memory address is " << long(slot_memory_address) << std::endl;
    #endif

    return slot_memory_address;
}

std::pair<FrameMetadata, char*> RingBuffer::read()
{
    FrameMetadata frame_metadata;

    while (1) {

        frame_metadata_queue_mutex.lock();

        if (frame_metadata_queue.empty()) {
            frame_metadata_queue_mutex.unlock();
            continue;

        } else {
            frame_metadata = frame_metadata_queue.front();
            frame_metadata_queue.pop_front();

            frame_metadata_queue_mutex.unlock();

             #ifdef DEBUG
                std::cout << "[RingBuffer] Received metadata for frame_index " << frame_metadata.frame_index << std::endl;
            #endif
        }

        // Check if the references ring buffer slot is valid.
        ringbuffer_slots_mutex.lock();

        if (!ringbuffer_slots[frame_metadata.buffer_slot_index]) {
            std::stringstream error_message;
            error_message << "Ring buffer slot referenced in message header " << frame_metadata.buffer_slot_index << " is empty." << std::endl;

            throw std::runtime_error(error_message.str());
        }

        ringbuffer_slots_mutex.unlock();

        // Memory address of frame in buffer.
        char* slot_memory_address = get_buffer_slot_address(frame_metadata.buffer_slot_index);
                
        return {frame_metadata, slot_memory_address};
    };
}

void RingBuffer::release(size_t buffer_slot_index) {
    // Cannot release a slot index that is out of range.
    if (buffer_slot_index >= n_slots) {
        std::stringstream error_message;
        error_message << "Slot index to release " << buffer_slot_index << " is out of range. Ring buffer n_slots = " << n_slots << std::endl;

        throw std::runtime_error(error_message.str());
    }

    // Release the buffer slot.
    ringbuffer_slots_mutex.lock();

    if (ringbuffer_slots[buffer_slot_index]) {
        ringbuffer_slots[buffer_slot_index] = 0;

    } else {
        std::stringstream error_message;
        error_message << "Cannot release empty ring buffer slot " << buffer_slot_index << std::endl;

        throw std::runtime_error(error_message.str());
    }

    ringbuffer_slots_mutex.unlock();
}