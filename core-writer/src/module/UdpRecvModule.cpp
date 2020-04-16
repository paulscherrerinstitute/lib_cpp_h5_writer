#include "UdpRecvModule.hpp"
#include "jungfrau.hpp"
#include <iostream>
#include <UdpReceiver.hpp>

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

        jungfrau_packet packet_buffer;

        while (is_receiving_.load(memory_order_relaxed)) {

            if (!udp_receiver.receive(
                    &packet_buffer,
                    JUNGFRAU_BYTES_PER_PACKET)) {
                continue;
            }

            auto* frame_metadata = metadata.get();

            // TODO: Horrible. Breake it down into methods.

            // First packet for this frame.
            if (frame_metadata->pulse_id == 0) {
                frame_metadata->frame_index = packet_buffer.framenum;
                frame_metadata->pulse_id = packet_buffer.bunchid;
                frame_metadata->recv_packets_1 = ~(uint64_t)0;
                frame_metadata->recv_packets_2 = ~(uint64_t)0;
                frame_metadata->daq_rec = packet_buffer.debug;
            // Packet from new frame, while we lost the last packet of
            // previous frame.
            } else if (frame_metadata->pulse_id != packet_buffer.bunchid) {
                ring_buffer_.commit(metadata);

                metadata = make_shared<UdpFrameMetadata>();
                metadata->frame_bytes_size = JUNGFRAU_DATA_BYTES_PER_FRAME;
                metadata->pulse_id = 0;
                metadata->n_recv_packets = 0;

                frame_buffer = ring_buffer_.reserve(metadata);
                memset(frame_buffer, 0, JUNGFRAU_DATA_BYTES_PER_FRAME);

                frame_metadata->frame_index = packet_buffer.framenum;
                frame_metadata->pulse_id = packet_buffer.bunchid;
                frame_metadata->recv_packets_1 = ~(uint64_t)0;
                frame_metadata->recv_packets_2 = ~(uint64_t)0;
                frame_metadata->daq_rec = packet_buffer.debug;
            }

            size_t frame_buffer_offset =
                    JUNGFRAU_DATA_BYTES_PER_PACKET * packet_buffer.packetnum;

            memcpy(
                    (void*) (frame_buffer + frame_buffer_offset),
                    packet_buffer.data,
                    JUNGFRAU_DATA_BYTES_PER_PACKET);

            frame_metadata->n_recv_packets++;

            if (packet_buffer.packetnum < 64) {
                frame_metadata->recv_packets_1 ^=
                        (uint64_t)1 << packet_buffer.packetnum;
            } else {
                frame_metadata->recv_packets_2 ^=
                        (uint64_t)1 << (packet_buffer.packetnum - 64);
            }

            // Frame finished with last packet.
            if (packet_buffer.packetnum == JUNGFRAU_N_PACKETS_PER_FRAME-1)
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