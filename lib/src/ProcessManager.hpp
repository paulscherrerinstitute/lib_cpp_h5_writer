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

    void notify_first_pulse_id(uint64_t pulse_id);
    void notify_last_pulse_id(uint64_t pulse_id);

    public:
        ProcessManager(WriterManager& writer_manager, ZmqReceiver& receiver, 
            RingBuffer& ring_buffer, const H5Format& format, uint16_t rest_port, const std::string& bsread_rest_address, hsize_t frames_per_file=0);

        void run_writer();

        void receive_zmq();

        void write_h5();

};

#endif