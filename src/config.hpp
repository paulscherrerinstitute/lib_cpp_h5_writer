#include <H5Cpp.h>
#include <string>

#ifndef CONFIG_H
#define CONFIG_H

namespace config
{
    extern int zmq_n_io_threads;
    extern int zmq_receive_timeout;
    extern int zmq_buffer_size_header;
    extern int zmq_buffer_size_data;

    extern size_t ring_buffer_n_slots;
    extern uint32_t ring_buffer_read_retry_interval;

    extern hsize_t dataset_increase_step;
    extern hsize_t initial_dataset_size;
}

#endif