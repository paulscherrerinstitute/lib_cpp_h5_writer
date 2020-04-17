#include "UdpRecvModule.hpp"
#include "jungfrau.hpp"
#include <iostream>
#include <UdpReceiver.hpp>
#include <netinet/in.h>

using namespace std;

UdpRecvModule::UdpRecvModule(RingBuffer<UdpFrameMetadata>& ring_buffer) :
            ring_buffer_(ring_buffer),
            is_receiving_(false)
{

}

UdpRecvModule::~UdpRecvModule()
{
    stop_recv();
}

void UdpRecvModule::start_recv(
        const uint16_t udp_port,
        const size_t frame_n_bytes)
{
    if (is_receiving_ == true) {
        std::stringstream err_msg;

        using namespace date;
        using namespace chrono;
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[UdpRecvModule::start_recv]";
        err_msg << " Receivers already running." << endl;

        throw runtime_error(err_msg.str());
    }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[UdpRecvModule::start_recv]";
        cout << " Starting with ";
        cout << "udp_port " << udp_port << endl;
    #endif

    is_receiving_ = true;

    if (receiving_thread_.joinable()) {
        receiving_thread_.join();
    }

    receiving_thread_ = thread(
            &UdpRecvModule::receive_thread, this,
            udp_port,
            frame_n_bytes);
}

void UdpRecvModule::stop_recv()
{
#ifdef DEBUG_OUTPUT
    using namespace date;
    using namespace chrono;
    cout << "[" << system_clock::now() << "]";
    cout << "UdpRecvModule::stop_recv";
    cout << " Stop receiving." << endl;
#endif

    is_receiving_ = false;

    if (receiving_thread_.joinable()) {
        receiving_thread_.join();
    }
}

void UdpRecvModule::receive_thread(
        const uint16_t udp_port,
        const size_t frame_size)
{
    try {
        ring_buffer_.initialize(frame_size);

        UdpReceiver udp_receiver;
        udp_receiver.bind(udp_port);

        auto metadata = make_shared<UdpFrameMetadata>();
        metadata->frame_bytes_size = JUNGFRAU_DATA_BYTES_PER_FRAME;
        metadata->pulse_id = 0;
        metadata->n_recv_packets = 0;

        char* frame_buffer = ring_buffer_.reserve(metadata);

        auto n_msgs = JUNGFRAU_N_PACKETS_PER_FRAME;
        jungfrau_packet recv_buffers[n_msgs];
        iovec recv_buff_ptr[n_msgs];
        struct mmsghdr msgs[n_msgs];
        struct sockaddr_in sockFrom[n_msgs];

        for (int i = 0; i < n_msgs; i++) {
            recv_buff_ptr[i].iov_base = (void*) &(recv_buffers[i]);
            recv_buff_ptr[i].iov_len = sizeof(jungfrau_packet);

            msgs[i].msg_hdr.msg_iov = &recv_buff_ptr[i];
            msgs[i].msg_hdr.msg_iovlen = 1;
            msgs[i].msg_hdr.msg_name = &sockFrom[i];
            msgs[i].msg_hdr.msg_namelen = sizeof(struct sockaddr_in );
        }

        while (is_receiving_.load(memory_order_relaxed)) {

            auto n_packets = udp_receiver.receive_many(msgs, n_msgs);

            if (n_packets < 1) {
                continue;
            }

            for (int i_packet=0; i_packet<n_packets; i_packet++) {

                auto* frame_metadata = metadata.get();
                jungfrau_packet* packet_buffer = &(recv_buffers[i_packet]);

                // TODO: Horrible. Breake it down into methods.

                // First packet for this frame.
                if (frame_metadata->pulse_id == 0) {
                    frame_metadata->frame_index = packet_buffer->framenum;
                    frame_metadata->pulse_id = packet_buffer->bunchid;
                    frame_metadata->daq_rec = packet_buffer->debug;
                    // Packet from new frame, while we lost the last packet of
                    // previous frame.

                } else if (frame_metadata->pulse_id != packet_buffer->bunchid) {
                    ring_buffer_.commit(metadata);

                    metadata = make_shared<UdpFrameMetadata>();
                    metadata->frame_bytes_size = JUNGFRAU_DATA_BYTES_PER_FRAME;
                    metadata->pulse_id = 0;
                    metadata->n_recv_packets = 0;

                    frame_buffer = ring_buffer_.reserve(metadata);
                    memset(frame_buffer, 0, JUNGFRAU_DATA_BYTES_PER_FRAME);

                    frame_metadata->frame_index = packet_buffer->framenum;
                    frame_metadata->pulse_id = packet_buffer->bunchid;
                    frame_metadata->daq_rec = packet_buffer->debug;
                }

                size_t frame_buffer_offset =
                        JUNGFRAU_DATA_BYTES_PER_PACKET * packet_buffer->packetnum;

                memcpy(
                        (void*) (frame_buffer + frame_buffer_offset),
                        packet_buffer->data,
                        JUNGFRAU_DATA_BYTES_PER_PACKET);

                frame_metadata->n_recv_packets++;

                // Frame finished with last packet.
                if (packet_buffer->packetnum == JUNGFRAU_N_PACKETS_PER_FRAME-1)
                {
                    ring_buffer_.commit(metadata);

                    metadata = make_shared<UdpFrameMetadata>();
                    metadata->frame_bytes_size = JUNGFRAU_DATA_BYTES_PER_FRAME;
                    metadata->pulse_id = 0;
                    metadata->n_recv_packets = 0;

                    frame_buffer = ring_buffer_.reserve(metadata);
                    memset(frame_buffer, 0, JUNGFRAU_DATA_BYTES_PER_FRAME);
                }
            }
        }

    } catch (const std::exception& e) {
        is_receiving_ = false;

        using namespace date;
        using namespace chrono;

        cout << "[" << system_clock::now() << "]";
        cout << "[UdpRecvModule::receive_thread]";
        cout << " Stopped because of exception: " << endl;
        cout << e.what() << endl;

        throw;
    }
}