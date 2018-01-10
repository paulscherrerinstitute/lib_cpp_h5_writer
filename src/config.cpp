#include "config.hpp"

namespace config {
    // Receiver config.
    int zmq_n_io_threads = 1;
    int zmq_receive_timeout = 100;

    // Ring buffer config.
    size_t ring_buffer_n_slots = 100;
    uint32_t ring_buffer_read_retry_interval = 5;
    
    // H5 config.
    std::string dataset_name = "data";
    hsize_t dataset_increase_step = 1000;
    hsize_t initial_dataset_size = 1000;
    H5::PredType dataset_type = H5::PredType::NATIVE_UINT8;
    H5T_order_t dataset_byte_order = H5T_ORDER_LE;
}
