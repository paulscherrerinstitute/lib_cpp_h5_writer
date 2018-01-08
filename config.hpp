#include <H5Cpp.h>

#ifndef CONFIG
#define CONFIG

namespace config
{
    extern H5::PredType dataset_type;
    extern H5T_order_t dataset_byte_order;
    extern hsize_t dataset_increase_step;
    extern hsize_t initial_dataset_size;
}

#endif