#ifndef ZMQRECVMODULE_H
#define ZMQRECVMODULE_H

#include <thread>
#include "ZmqReceiver.hpp"
#include "RingBuffer.hpp"

class ZmqRecvModule
{
    typedef std::unordered_map<std::string, HeaderDataType> header_map;

    RingBuffer& ring_buffer_;
    const header_map& header_values_;
    const std::atomic_bool& is_writing_;
    std::atomic_bool is_receiving_;
    std::vector<std::thread> receiving_threads_;

protected:
    void receive_thread(
            const std::string& connect_address);

public:
    ZmqRecvModule(
            RingBuffer& ring_buffer,
            const header_map& header_values,
            const std::atomic_bool& is_writing);

    void start(const std::string& connect_address,
               const uint8_t n_receiving_threads);
    void stop();
};

#endif