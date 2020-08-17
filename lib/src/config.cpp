#include "config.hpp"

namespace config {
    // Number of receiving threads. Roughly 1 thread / (GB/s)
    int zmq_n_io_threads = 1;
    int zmq_receive_timeout = 100;
    
    // JSON header buffer size - 1MB.
    int zmq_buffer_size_header = 1024 * 1024 * 1;
    // Data message buffer size - 10MB.
    int zmq_buffer_size_data = 1024 * 1024 * 10;

    // Ring buffer config.
    // Allow for a couple of seconds (file creation might be slow).
    size_t ring_buffer_n_slots = 1000;
    // Delay before trying again to get data from the ring buffer.
    size_t ring_buffer_read_retry_interval = 5;

    // Delay to get statistics from the statistics writer buffer
    size_t statistics_buffer_adv_interval = 5;//milliseconds

    std::string raw_image_dataset_name = "raw_data";
    
    // By how much to enlarge a dataset when a resizing is needed.
    hsize_t dataset_increase_step = 1000;
    // To which value to initialize a dataset size.
    hsize_t initial_dataset_size = 1000;

    // Delay in between attempts to see if the requred parameters were passed over the REST api.
    uint32_t parameters_read_retry_interval = 300;
}
