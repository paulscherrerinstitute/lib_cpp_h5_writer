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
    std::atomic_bool is_receiving_;
    std::atomic_int n_frames_to_save_;
    std::vector<std::thread> receiving_threads_;

protected:
    void receive_thread(
            const std::string& connect_address);

public:
    ZmqRecvModule(
            RingBuffer& ring_buffer,
            const header_map& header_values);

    void start_recv(
            const std::string& connect_address,
            const uint8_t n_receiving_threads);
    void stop_recv();

    void start_saving(const int n_frames = -1);
    void stop_saving();
    bool is_saving();
};

#endif