#ifndef PROXYMANAGER_H
#define PROXYMANAGER_H

#include "WriterManager.hpp"
#include "H5Format.hpp"
#include "RingBuffer.hpp"
#include "ZmqReceiver.hpp"
#include <chrono>
#include "date.h"

class ProxyManager 
{
    WriterManager& writer_manager;
    ZmqReceiver& receiver;
    RingBuffer& ring_buffer;
    const H5Format& format;

    uint16_t rest_port;
    const std::string& bsread_rest_address;

    void notify_first_pulse_id(uint64_t pulse_id);
    void notify_last_pulse_id(uint64_t pulse_id);

    public:
        ProxyManager(WriterManager& writer_manager, ZmqReceiver& receiver, 
            RingBuffer& ring_buffer, const H5Format& format, uint16_t rest_port, const std::string& bsread_rest_address);

        void run_writer();

        void receive_zmq();

        void write_h5(size_t frames_per_file);

        void write_h5_format(H5::H5File& file);
};

#endif
