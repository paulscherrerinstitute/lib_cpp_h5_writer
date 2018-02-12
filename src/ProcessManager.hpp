#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include "WriterManager.hpp"
#include "H5Format.hpp"
#include "RingBuffer.hpp"
#include "ZmqReceiver.hpp"

namespace ProcessManager 
{
    void run_writer(WriterManager& manager, const H5Format& format, ZmqReceiver& receiver, uint16_t rest_port);

    void receive_zmq(WriterManager& manager, RingBuffer& ring_buffer, ZmqReceiver& receiver, const H5Format& format);

    void write_h5(WriterManager& manager, const H5Format& format, RingBuffer& ring_buffer);
};

#endif