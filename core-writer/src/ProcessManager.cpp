#include <cstdlib>
#include <chrono>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <boost/thread.hpp>
#include <future>
#include <vector>

#include "RestApi.hpp"
#include "ProcessManager.hpp"
#include "config.hpp"
#include "BufferedWriter.hpp"
#include "compression.h"

using namespace std;

ProcessManager::ProcessManager(
    WriterManager& writer_manager,
    ZmqReceiver& receiver,
    RingBuffer& ring_buffer, 
    const H5Format& format,
    uint16_t rest_port,
    const string& bsread_rest_address,
    hsize_t frames_per_file) :
        writer_manager(writer_manager),
        receiver(receiver),
        ring_buffer(ring_buffer),
        format(format),
        rest_port(rest_port), 
        bsread_rest_address(bsread_rest_address),
        frames_per_file(frames_per_file)
{
}


void ProcessManager::run_receivers(uint8_t n_receiving_threads)
{


    RestApi::start_rest_api(writer_manager, rest_port);


    // In case SIGINT stopped the rest_api.
    writer_manager.stop();


    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ProcessManager::run_writer] Writer properly stopped." << endl;
    #endif
}

void ProcessManager::receive_zmq()
{
    receiver.connect();

    while (writer_manager.is_running()) {
        
        auto frame = receiver.receive();
        
        // In case no message is available before the timeout, both pointers are NULL.
        if (!frame.first || !writer_manager.receive_frame()){
            continue;
        }

        auto frame_metadata = frame.first;
        auto frame_data = frame.second;

        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << std::chrono::system_clock::now() << "]";
            cout << "[ProcessManager::receive_zmq] Processing FrameMetadata"; 
            cout << " with frame_index " << frame_metadata->frame_index;
            cout << " and frame_shape [" << frame_metadata->frame_shape[0] << ", " << frame_metadata->frame_shape[1] << "]";
            cout << " and endianness " << frame_metadata->endianness;
            cout << " and type " << frame_metadata->type;
            cout << " and frame_bytes_size " << frame_metadata->frame_bytes_size;
            cout << "." << endl;
        #endif

        char* buffer = ring_buffer.reserve(frame_metadata);

        size_t max_buffer_size = compression::get_bitshuffle_max_buffer_size(
            frame_metadata->frame_bytes_size, 1);

        if (max_buffer_size > ring_buffer.get_slot_size()) {
             
        }
    
        auto compressed_size = compression::compress_bitshuffle(
            static_cast<const char*>(frame_data),
            frame_metadata->frame_bytes_size,
            1,
            buffer);

        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << std::chrono::system_clock::now() << "]";
            cout << "[ProcessManager::receive_zmq] Compressed image from "; 
            cout << frame_metadata->frame_bytes_size << " bytes to "; 
            cout << compressed_size << " bytes." << endl; 
        #endif

        frame_metadata->frame_bytes_size = compressed_size;

        ring_buffer.commit(frame_metadata);
   }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ProcessManager::receive_zmq] Receiver thread stopped." << endl;
    #endif
}

