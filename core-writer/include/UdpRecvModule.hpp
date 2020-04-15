#ifndef UDPRECVMODULE_HPP
#define UDPRECVMODULE_HPP

#include "RingBuffer.hpp"
#include <thread>

class UdpRecvModule {

        RingBuffer<UdpFrameMetadata>& ring_buffer_;

        std::atomic_bool is_receiving_;
        std::thread receiving_thread_;

    protected:
        void receive_thread(
                const uint16_t udp_port,
                const size_t frame_size);

    public:
        UdpRecvModule(RingBuffer<UdpFrameMetadata>& ring_buffer);

        virtual ~UdpRecvModule();

        void start_recv(
                const uint16_t udp_port,
                const size_t frame_n_bytes);
        void stop_recv();
        bool is_receiving();
};


#endif // UDPRECVMODULE_HPP
