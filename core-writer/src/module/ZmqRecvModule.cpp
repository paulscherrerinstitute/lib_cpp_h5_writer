
#include <config.hpp>
#include <iostream>
#include <compression.hpp>
#include "ZmqRecvModule.hpp"

using namespace std;

ZmqRecvModule::ZmqRecvModule(
        RingBuffer<FrameMetadata> &ring_buffer,
        const header_map &header_values) :
            ring_buffer_(ring_buffer),
            header_values_(header_values),
            is_receiving_(false),
            is_saving_(false)
{}

ZmqRecvModule::~ZmqRecvModule()
{
    stop_recv();
}

void ZmqRecvModule::start_recv(
        const string& connect_address,
        const uint8_t n_receiving_threads)
{
    if (is_receiving_ == true) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono;
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[ZmqRecvModule::start_recv]";
        err_msg << " Receivers already running." << endl;

        throw runtime_error(err_msg.str());
    }

    if (n_receiving_threads < 1) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono;
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[ZmqRecvModule::start_recv]";
        err_msg << " n_receiving_threads ";
        err_msg << n_receiving_threads << " must be > 1." << endl;

        throw runtime_error(err_msg.str());
    }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ZmqRecvModule::start]";
        cout << " Starting with ";
        cout << "connect_address " << connect_address;
        cout << " n_receiving_thread ";
        cout << (int) n_receiving_threads << endl;
    #endif

    is_receiving_ = true;

    for (uint8_t i_rec=0; i_rec < n_receiving_threads; i_rec++) {
        receiving_threads_.emplace_back(
                &ZmqRecvModule::receive_thread, this, connect_address);
    }
}

void ZmqRecvModule::stop_recv()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ZmqRecvModule::stop_recv]";
        cout << " Stop receiving threads." << endl;
    #endif

    is_receiving_ = false;

    for (auto& recv_thread:receiving_threads_) {
        if (recv_thread.joinable()) {
            recv_thread.join();
        }
    }

    receiving_threads_.clear();
}

bool ZmqRecvModule::is_receiving()
{
    return is_receiving_;
}

void ZmqRecvModule::start_saving()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ZmqRecvModule::start_saving]";
        cout << " Enable saving." << endl;
    #endif

    is_saving_ = true;
}

void ZmqRecvModule::stop_saving_and_clear_buffer()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ZmqRecvModule::stop_saving]";
        cout << " Disable saving." << endl;
    #endif

    is_saving_ = false;

    // TODO: Solve the problem differently - control to RB?
    this_thread::sleep_for(chrono::milliseconds(
            config::recv_saving_wait_ms));

    ring_buffer_.clear();
}

void ZmqRecvModule::receive_thread(const string& connect_address)
{
    try {

        ZmqReceiver receiver(header_values_);
        receiver.connect(connect_address);

        bool rb_initialized(false);

        while (is_receiving_.load(memory_order_relaxed)) {

            auto frame = receiver.receive();

            // .first and .second = nullptr when no message received
            if (frame.first == nullptr ) {
                continue;
            }

            if (!is_saving_) {
                continue;
            }

            auto frame_metadata = frame.first;
            auto frame_data = frame.second;

            #ifdef DEBUG_OUTPUT
                using namespace date;
                using namespace chrono;
                cout << "[" << system_clock::now() << "]";
                cout << "[ZmqRecvModule::receive_thread]";
                cout << " Processing FrameMetadata with frame_index ";
                cout << frame_metadata->frame_index;
                cout << " and frame_shape [" << frame_metadata->frame_shape[0];
                cout << ", " << frame_metadata->frame_shape[1] << "]";
                cout << " and endianness " << frame_metadata->endianness;
                cout << " and type " << frame_metadata->type;
                cout << " and frame_bytes_size ";
                cout << frame_metadata->frame_bytes_size << "." << endl;
            #endif

            if (!rb_initialized) {

                size_t n_elements =
                        frame_metadata->frame_shape[0] *
                        frame_metadata->frame_shape[1];

                size_t max_buffer_size =
                        compression::get_bitshuffle_max_buffer_size(
                                n_elements,
                                frame_metadata->frame_bytes_size/n_elements);

                ring_buffer_.initialize(max_buffer_size);

                rb_initialized = true;
            }

            char* buffer = ring_buffer_.reserve(frame_metadata);
            memcpy(
                    buffer,
                    static_cast<const char*>(frame_data),
                    frame_metadata->frame_bytes_size);

//            auto compressed_size = compression::compress_bitshuffle(
//                    static_cast<const char*>(frame_data),
//                    frame_metadata->frame_bytes_size,
//                    1,
//                    buffer);
//
//            #ifdef DEBUG_OUTPUT
//                using namespace date;
//                using namespace chrono;
//                cout << "[" << system_clock::now() << "]";
//                cout << "[ZmqRecvModule::receive_thread]";
//                cout << " Compressed image from ";
//                cout << frame_metadata->frame_bytes_size << " bytes to ";
//                cout << compressed_size << " bytes." << endl;
//            #endif

//            frame_metadata->frame_bytes_size = compressed_size;

            ring_buffer_.commit(frame_metadata);
        }

        receiver.disconnect();

        #ifdef DEBUG_OUTPUT
            using namespace date;
            using namespace chrono;
            cout << "[" << system_clock::now() << "]";
            cout << "[ZmqRecvModule::receive_thread]";
            cout << " Receiver thread stopped." << endl;
        #endif

    } catch (const std::exception& e) {
        is_receiving_ = false;

        using namespace date;
        using namespace chrono;

        cout << "[" << system_clock::now() << "]";
        cout << "[ZmqRecvModule::receive_thread]";
        cout << " Stopped because of exception: " << endl;
        cout << e.what() << endl;

        throw;
    }
}
