#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include "WriterManager.hpp"
#include "H5Format.hpp"
#include "RingBuffer.hpp"
#include "ZmqReceiver.hpp"
#include <chrono>
#include "date.h"

namespace ProcessManager 
{
    void run_writer(WriterManager& manager, const H5Format& format, ZmqReceiver& receiver, uint16_t rest_port, 
        const std::string& bsread_rest_address);

    void receive_zmq(WriterManager& manager, RingBuffer& ring_buffer, ZmqReceiver& receiver, const H5Format& format);

    void write_h5(WriterManager& manager, const H5Format& format, RingBuffer& ring_buffer,
        const std::shared_ptr<std::unordered_map<std::string, HeaderDataType>> header_values_type, 
            const std::string& bsread_rest_address);

    void notify_first_pulse_id(const std::string& bsread_rest_address, uint64_t pulse_id);
    void notify_last_pulse_id(const std::string& bsread_rest_address, uint64_t pulse_id);
};

#endif