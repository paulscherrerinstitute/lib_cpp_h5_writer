
#include <config.hpp>
#include <iostream>
#include <compression.hpp>
#include "ZmqRecvModule.hpp"

using namespace std;

ZmqRecvModule::ZmqRecvModule(
        RingBuffer &ringBuffer,
        const header_map &header_values,
        const std::atomic_bool& is_writing) :
            ring_buffer_(ring_buffer_),
            header_values_(header_values),
            is_writing_(is_writing),
            is_receiving_(false)
{}

void ZmqRecvModule::start(
        const string& connect_address,
        const uint8_t n_receiving_thread)
{

}

void ZmqRecvModule::stop()
{

}

void ZmqRecvModule::receive_thread(
        const string& connect_address,
        const uint8_t n_receiving_threads)
{
    ZmqReceiver receiver(
            connect_address,
            config::zmq_n_io_threads,
            config::zmq_receive_timeout,
            header_values_);

    receiver.connect();

    while (is_receiving_.load(memory_order_relaxed)) {

        auto frame = receiver.receive();

        // If no message, first and second = nullptr
        if (frame.first == nullptr ||
            !is_writing_.load(memory_order_relaxed)) {
            continue;
        }

        auto frame_metadata = frame.first;
        auto frame_data = frame.second;

        #ifdef DEBUG_OUTPUT
            using namespace date;
            using namespace chrono;
            cout << "[" << system_clock::now() << "]";
            cout << "[ProcessManager::receive_zmq]";
            cout << " Processing FrameMetadata with frame_index ";
            cout << frame_metadata->frame_index;
            cout << " and frame_shape [" << frame_metadata->frame_shape[0];
            cout << ", " << frame_metadata->frame_shape[1] << "]";
            cout << " and endianness " << frame_metadata->endianness;
            cout << " and type " << frame_metadata->type;
            cout << " and frame_bytes_size ";
            cout << frame_metadata->frame_bytes_size << "." << endl;
        #endif

        char* buffer = ring_buffer_.reserve(frame_metadata);

        // TODO: Add flag to disable compression.
        {
            // TODO: Cache results no to calculate this every time.
            size_t max_buffer_size =
                    compression::get_bitshuffle_max_buffer_size(
                    frame_metadata->frame_bytes_size, 1);

            if (max_buffer_size > ring_buffer_.get_slot_size()) {
                //TODO: Throw error if not large enough.
            }
        }

        auto compressed_size = compression::compress_bitshuffle(
                static_cast<const char*>(frame_data),
                frame_metadata->frame_bytes_size,
                1,
                buffer);

        #ifdef DEBUG_OUTPUT
            using namespace date;
            using namespace chrono;
            cout << "[" << system_clock::now() << "]";
            cout << "[ProcessManager::receive_zmq]";
            cout << " Compressed image from ";
            cout << frame_metadata->frame_bytes_size << " bytes to ";
            cout << compressed_size << " bytes." << endl;
        #endif

        frame_metadata->frame_bytes_size = compressed_size;

        ring_buffer_.commit(frame_metadata);
    }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ProcessManager::receive_zmq]";
        cout << " Receiver thread stopped." << endl;
    #endif
}
