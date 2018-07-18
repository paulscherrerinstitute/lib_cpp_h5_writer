#include <cstdlib>
#include <chrono>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <boost/thread.hpp>
#include <future>

#include "RestApi.hpp"
#include "ProcessManager.hpp"
#include "config.hpp"
#include "H5Writer.hpp"

using namespace std;

void ProcessManager::notify_first_pulse_id(const string& bsread_rest_address, uint64_t pulse_id) 
{
    // First pulse_id should be an async operation - we do not want to make the writer wait.
    async(launch::async, [pulse_id, &bsread_rest_address]{
        try {

            cout << "Sending first received pulse_id " << pulse_id << " to bsread_rest_address " << bsread_rest_address << endl;

            stringstream request;
            request << "curl -X PUT " << bsread_rest_address << "/start_pulse_id/" << pulse_id;

            string request_call(request.str());

            #ifdef DEBUG_OUTPUT
                using namespace date;
                cout << "[" << std::chrono::system_clock::now() << "]";
                cout << "[SfProcessManager::notify_first_pulse_id] Sending request (" << request_call << ")." << endl;
            #endif

            system(request_call.c_str());
        } catch (...){}
        
    });
}

void ProcessManager::notify_last_pulse_id(const string& bsread_rest_address, uint64_t pulse_id) 
{
    // Last pulse_id should be a sync operation - we do not want to terminate the process to quickly.
    cout << "Sending last received pulse_id " << pulse_id << " to bsread address " << bsread_rest_address << endl;

    try {
        stringstream request;
        request << "curl -X PUT " << bsread_rest_address << "/stop_pulse_id/" << pulse_id;

        cout << "Request: " << request.str() << endl;

        string request_call(request.str());

        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << std::chrono::system_clock::now() << "]";
            cout << "[SfProcessManager::notify_last_pulse_id] Sending request (" << request_call << ")." << endl;
        #endif

        system(request_call.c_str());
    } catch (...){}
}

void ProcessManager::run_writer(WriterManager& manager, const H5Format& format, 
    ZmqReceiver& receiver, uint16_t rest_port, const string& bsread_rest_address)
{
    size_t n_slots = config::ring_buffer_n_slots;
    RingBuffer ring_buffer(n_slots);

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ProcessManager::run_writer] Running writer";
        cout << " and output_file " << manager.get_output_file();
        cout << " and n_slots " << n_slots;
        cout << endl;
    #endif

    boost::thread receiver_thread(receive_zmq, boost::ref(manager), boost::ref(ring_buffer), 
        boost::ref(receiver), boost::ref(format));
    boost::thread writer_thread(write_h5, boost::ref(manager), 
        boost::ref(format), boost::ref(ring_buffer), receiver.get_header_values_type(), boost::ref(bsread_rest_address));

    RestApi::start_rest_api(manager, rest_port);

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ProcessManager::run_writer] Rest API stopped." << endl;
    #endif

    // In case SIGINT stopped the rest_api.
    manager.stop();

    receiver_thread.join();
    writer_thread.join();

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
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

        // Commit the frame to the buffer.
        ring_buffer.write(frame_metadata, frame_data);

        manager.received_frame(frame_metadata->frame_index);
   }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ProcessManager::receive_zmq] Receiver thread stopped." << endl;
    #endif
}

void ProcessManager::write_h5(WriterManager& manager, const H5Format& format, RingBuffer& ring_buffer,
    const shared_ptr<unordered_map<string, HeaderDataType>> header_values_type, const string& bsread_rest_address)
{
    auto writer = get_h5_writer(manager.get_output_file(), 0, config::initial_dataset_size, config::dataset_increase_step);
    auto raw_frames_dataset_name = config::raw_image_dataset_name;

    uint64_t last_pulse_id = 0;
    
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

        #ifdef PERF_OUTPUT
            using namespace date;
            auto start_time_frame = std::chrono::system_clock::now();
        #endif

        // Write image data.
        writer->write_data(raw_frames_dataset_name,
                           received_data.first->frame_index, 
                           received_data.second,
                           received_data.first->frame_shape,
                           received_data.first->frame_bytes_size, 
                           received_data.first->type,
                           received_data.first->endianness);

        #ifdef PERF_OUTPUT
            using namespace date;
            using namespace std::chrono;

            auto frame_time_difference = std::chrono::system_clock::now() - start_time_frame;
            auto frame_diff_ms = duration<float, milli>(frame_time_difference).count();

            cout << "[" << std::chrono::system_clock::now() << "]";
            cout << "[ProcessManager::write_h5] Frame index "; 
            cout << received_data.first->frame_index << " written in " << frame_diff_ms << " ms." << endl;
        #endif

        ring_buffer.release(received_data.first->buffer_slot_index);

        #ifdef PERF_OUTPUT
            using namespace date;
            auto start_time_metadata = std::chrono::system_clock::now();
        #endif

        // Write image metadata if mapping specified.
        if (header_values_type) {

            for (const auto& header_type : *header_values_type) {

                auto& name = header_type.first;
                auto& header_data_type = header_type.second;

                auto value = received_data.first->header_values.at(name);

                // TODO: Ugly hack until we get the start sequence in the bsread stream itself.
                if (name == "pulse_id") {
                    if (!last_pulse_id) {
                        last_pulse_id = *(reinterpret_cast<uint64_t*>(value.get()));
                        notify_first_pulse_id(bsread_rest_address, last_pulse_id);
                    } else {
                        last_pulse_id = *(reinterpret_cast<uint64_t*>(value.get()));
                    }
                }

                // Header data are fixed to scalars in little endian.
                vector<size_t> value_shape = {header_data_type.value_shape};

                writer->write_data(name,
                                  received_data.first->frame_index,
                                  value.get(),
                                  value_shape,
                                  header_data_type.value_bytes_size,
                                  header_data_type.type,
                                  header_data_type.endianness);
            }
        }

        #ifdef PERF_OUTPUT
            using namespace date;
            using namespace std::chrono;

            auto metadata_time_difference = std::chrono::system_clock::now() - start_time_metadata;
            auto metadata_diff_ms = duration<float, milli>(metadata_time_difference).count();

            cout << "[" << std::chrono::system_clock::now() << "]";
            cout << "[ProcessManager::write_h5] Frame metadata index "; 
            cout << received_data.first->frame_index << " written in " << metadata_diff_ms << " ms." << endl;
        #endif
        
        manager.written_frame(received_data.first->frame_index);
    }

    // Send the last_pulse_id only if it was set.
    if (last_pulse_id) {
        notify_last_pulse_id(bsread_rest_address, last_pulse_id);
    }

    if (writer->is_file_open()) {
        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << std::chrono::system_clock::now() << "]";
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
                H5FormatUtils::write_format(writer->get_h5_file(), format, parameters);
            } catch (const runtime_error& ex) {
                using namespace date;
                std::cout << "[" << std::chrono::system_clock::now() << "]";
                std::cout << "[ProcessManager::write] Error while trying to write file format: "<< ex.what() << endl;
            }
        }
    }
    
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ProcessManager::write] Closing file " << manager.get_output_file() << endl;
    #endif
    
    writer->close_file();

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ProcessManager::write] Writer thread stopped." << endl;
    #endif

    // Exit when writer thread has closed the file.
    exit(0);
}