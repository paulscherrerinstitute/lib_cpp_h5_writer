#include "config.hpp"

namespace config {
    // Receiver config.
    int zmq_n_io_threads = 1;
    int zmq_receive_timeout = 100;
    int zmq_buffer_size_header = 1024 * 1024 * 0.2;
    int zmq_buffer_size_data = 1024 * 1024 * 10;

    // Ring buffer config.
    size_t ring_buffer_n_slots = 100;
    uint32_t ring_buffer_read_retry_interval = 5;
    
    // H5 config.
    hsize_t dataset_increase_step = 1000;
    hsize_t initial_dataset_size = 1000;
}
