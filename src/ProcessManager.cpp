#include <cstdlib>
#include <chrono>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <boost/thread.hpp>

#include "RestApi.hpp"
#include "ProcessManager.hpp"
#include "config.hpp"
#include "H5Writer.hpp"

using namespace std;

void ProcessManager::run_writer(WriterManager& manager, const H5Format& format, 
    ZmqReceiver& receiver, uint16_t rest_port)
{
    size_t n_slots = config::ring_buffer_n_slots;
    RingBuffer ring_buffer(n_slots);

    #ifdef DEBUG_OUTPUT
        cout << "[ProcessManager::run_writer] Running writer";
        cout << " and output_file " << manager.get_output_file();
        cout << " and n_slots " << n_slots;
        cout << endl;
    #endif

    boost::thread receiver_thread(receive_zmq, boost::ref(manager), boost::ref(ring_buffer), 
        boost::ref(receiver), boost::ref(format));
    boost::thread writer_thread(write_h5, boost::ref(manager), 
        boost::ref(format), boost::ref(ring_buffer), boost::ref(*receiver.get_header_values_type()));

    RestApi::start_rest_api(manager, rest_port);

    #ifdef DEBUG_OUTPUT
        cout << "[ProcessManager::run_writer] Rest API stopped." << endl;
    #endif

    // In case SIGINT stopped the rest_api.
    manager.stop();

    receiver_thread.join();
    writer_thread.join();

    #ifdef DEBUG_OUTPUT
        cout << "[ProcessManager::run_writer] Writer properly stopped." << endl;
    #endif
}

void ProcessManager::receive_zmq(WriterManager& manager, RingBuffer& ring_buffer,
    ZmqReceiver& receiver, const H5Format& format)
{
    receiver.connect();

    while (manager.is_running()) {
        
        auto frame = receiver.receive();
        
        // In case no message is available before the timeout, both pointers are NULL.
        if (!frame.first){
            continue;
        }

        auto frame_metadata = frame.first;
        auto frame_data = frame.second;

        #ifdef DEBUG_OUTPUT
            cout << "[ProcessManager::receive_zmq] Processing FrameMetadata"; 
            cout << " with frame_index " << frame_metadata->frame_index;
            cout << " and frame_shape [" << frame_metadata->frame_shape[0] << ", " << frame_metadata->frame_shape[1] << "]";
            cout << " and endianness " << frame_metadata->endianness;
            cout << " and type " << frame_metadata->type;
            cout << " and frame_bytes_size " << frame_metadata->frame_bytes_size;
            cout << "." << endl;
        #endif

        // Commit the frame to the buffer.
        ring_buffer.write(frame_metadata, frame_data);

        manager.received_frame(frame_metadata->frame_index);
   }

    #ifdef DEBUG_OUTPUT
        cout << "[ProcessManager::receive_zmq] Receiver thread stopped." << endl;
    #endif
}

void ProcessManager::write_h5(WriterManager& manager, const H5Format& format, RingBuffer& ring_buffer,
    const unordered_map<string, string>& header_values_type)
{
    H5Writer writer(manager.get_output_file(), 0, config::initial_dataset_size, config::dataset_increase_step);
    auto raw_frames_dataset_name = format.get_raw_frames_dataset_name();
    
    // Run until the running flag is set or the ring_buffer is empty.  
    while(manager.is_running() || !ring_buffer.is_empty()) {
        
        if (ring_buffer.is_empty()) {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(config::ring_buffer_read_retry_interval));
            continue;
        }

        const pair< shared_ptr<FrameMetadata>, char* > received_data = ring_buffer.read();
        
        // NULL pointer means that the ringbuffer->read() timeouted. Faster than rising an exception.
        if(!received_data.first) {
            continue;
        }

        // Write image data.
        writer.write_data(raw_frames_dataset_name,
                          received_data.first->frame_index, 
                          received_data.second,
                          received_data.first->frame_shape,
                          received_data.first->frame_bytes_size, 
                          received_data.first->type,
                          received_data.first->endianness);

        ring_buffer.release(received_data.first->buffer_slot_index);

        // Write image metadata.
        for (const auto& header_type : header_values_type) {
            auto& name = header_type.first;
            auto& type = header_type.second;

            auto value = received_data.first->header_values.at(name);

            writer.write_data(name,
                              received_data.first->frame_index,
                              value.get(),
                              {1},
                              4,
                              type,
                              "little");
        }

        manager.written_frame(received_data.first->frame_index);
    }

    if (writer.is_file_open()) {
        #ifdef DEBUG_OUTPUT
            cout << "[ProcessManager::write] Writing file format." << endl;
        #endif

        // Wait until all parameters are set or writer is killed.
        while (!manager.are_all_parameters_set() && !manager.is_killed()) {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(config::parameters_read_retry_interval));
        }

        // Need to check again if we have all parameters to write down the format.
        if (manager.are_all_parameters_set()) {
            const auto parameters = manager.get_parameters();
            
            // Even if we can't write the format, lets try to preserve the data.
            try {
                H5FormatUtils::write_format(writer.get_h5_file(), format, parameters);
            } catch (const runtime_error& ex) {
                cerr << "[ProcessManager::write] Error while trying to write file format: "<< ex.what() << endl;
            }
        }
    }
    
    #ifdef DEBUG_OUTPUT
        cout << "[ProcessManager::write] Closing file " << manager.get_output_file() << endl;
    #endif
    
    writer.close_file();

    #ifdef DEBUG_OUTPUT
        cout << "[ProcessManager::write] Writer thread stopped." << endl;
    #endif

    // Exit when writer thread has closed the file.
    exit(0);
}