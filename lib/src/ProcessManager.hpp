#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include "WriterManager.hpp"
#include "H5Format.hpp"
#include "RingBuffer.hpp"
#include "ZmqReceiver.hpp"
#include <chrono>
#include "date.h"

class ProcessManager 
{
    WriterManager& writer_manager;
    ZmqReceiver& receiver;
    RingBuffer& ring_buffer;
    const H5Format& format;

    uint16_t rest_port;
    const std::string& bsread_rest_address;
    hsize_t frames_per_file;

    public:
        ProcessManager(WriterManager& writer_manager, 
                       ZmqReceiver& receiver, 
                       RingBuffer& ring_buffer, 
                       const H5Format& format, 
                       uint16_t rest_port, 
                       const std::string& bsread_rest_address, 
                       hsize_t frames_per_file=0);

        void run_receivers(uint8_t n_receiving_threads);

};

#endif
