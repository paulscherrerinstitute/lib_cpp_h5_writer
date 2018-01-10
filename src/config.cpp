#include "config.hpp"

namespace config {
    H5::PredType dataset_type = H5::PredType::NATIVE_UINT8;
    H5T_order_t dataset_byte_order = H5T_ORDER_LE;
    hsize_t dataset_increase_step = 1000;
    hsize_t initial_dataset_size = 1000;

    uint16_t rest_port = 11011;
    int n_io_threads = 1;
    size_t n_slots = 100;

}
