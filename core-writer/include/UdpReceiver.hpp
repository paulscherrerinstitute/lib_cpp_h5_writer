#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include "config.hpp"
#include <sys/socket.h>
#include "RingBuffer.hpp"

class UdpReceiver {

    int socket_fd_;

public:
    UdpReceiver();
    virtual ~UdpReceiver();

    bool receive(void* buffer, size_t buffer_n_bytes);
    int receive_many(mmsghdr* msgs, const size_t n_msgs);

    void bind(
            const uint16_t port,
            const size_t usec_timeout=config::udp_usec_timeout);
    void close();
};


#endif //LIB_CPP_H5_WRITER_UDPRECEIVER_H
