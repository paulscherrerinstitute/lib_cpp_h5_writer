#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include "config.hpp"
#include "RingBuffer.hpp"

class UdpReceiver {

    int socket_fd_;

public:
    UdpReceiver();
    virtual ~UdpReceiver();

    bool receive(void* buffer, size_t buffer_n_bytes);
    void bind(
            const uint16_t port,
            const size_t usec_timeout=config::udp_usec_timeout);
    void close();
};


#endif //LIB_CPP_H5_WRITER_UDPRECEIVER_H
