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
#include "BufferedWriter.hpp"

using namespace std;

ProcessManager::ProcessManager(WriterManager& writer_manager, ZmqReceiver& receiver, RingBuffer& ring_buffer, 
    const H5Format& format, uint16_t rest_port, const string& bsread_rest_address, hsize_t frames_per_file) :
        writer_manager(writer_manager), receiver(receiver), ring_buffer(ring_buffer), format(format), rest_port(rest_port), 
        bsread_rest_address(bsread_rest_address), frames_per_file(frames_per_file)
{
}

void ProcessManager::notify_first_pulse_id(uint64_t pulse_id) 
{
    string request_address(bsread_rest_address);

    // First pulse_id should be an async operation - we do not want to make the writer wait.
    async(launch::async, [pulse_id, &request_address]{
        try {

            cout << "Sending first received pulse_id " << pulse_id << " to bsread_rest_address " << request_address << endl;

            stringstream request;
            request << "curl -X PUT " << request_address << "/start_pulse_id/" << pulse_id;

            string request_call(request.str());

            #ifdef DEBUG_OUTPUT
                using namespace date;
                cout << "[" << std::chrono::system_clock::now() << "]";
                cout << "[ProcessManager::notify_first_pulse_id] Sending request (" << request_call << ")." << endl;
            #endif

            system(request_call.c_str());
        } catch (...){}
        
    });
}

void ProcessManager::notify_last_pulse_id(uint64_t pulse_id) 
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
            cout << "[ProcessManager::notify_last_pulse_id] Sending request (" << request_call << ")." << endl;
        #endif

        system(request_call.c_str());
    } catch (...){}
}

void ProcessManager::run_writer()
{

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ProcessManager::run_writer] Running writer";
        cout << " and output_file " << writer_manager.get_output_file();
        cout << endl;
    #endif

    boost::thread receiver_thread(&ProcessManager::receive_zmq, this);
    boost::thread writer_thread(&ProcessManager::write_h5, this);

    RestApi::start_rest_api(writer_manager, rest_port);

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ProcessManager::run_writer] Rest API stopped." << endl;
    #endif

    // In case SIGINT stopped the rest_api.
    writer_manager.stop();

    receiver_thread.join();
    writer_thread.join();

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

        writer_manager.received_frame(frame_metadata->frame_index);
   }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ProcessManager::receive_zmq] Receiver thread stopped." << endl;
    #endif
}

void ProcessManager::write_h5()
{
    size_t metadata_buffer_size = frames_per_file != 0 ? frames_per_file : writer_manager.get_n_frames();
    auto metadata_buffer = unique_ptr<MetadataBuffer>(new MetadataBuffer(metadata_buffer_size, receiver.get_header_values_type()));

    auto writer = get_buffered_writer(writer_manager.get_output_file(), writer_manager.get_n_frames(), move(metadata_buffer), 
        frames_per_file, config::dataset_increase_step);

    writer->create_file();
        
    auto raw_frames_dataset_name = config::raw_image_dataset_name;

    uint64_t last_pulse_id = 0;
    
    // Run until the running flag is set or the ring_buffer is empty.  
    while(writer_manager.is_running() || !ring_buffer.is_empty()) {
        
        if (ring_buffer.is_empty()) {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(config::ring_buffer_read_retry_interval));
            continue;
        }

        const pair< shared_ptr<FrameMetadata>, char* > received_data = ring_buffer.read();
        
        // NULL pointer means that the ringbuffer->read() timeouted. Faster than rising an exception.
        if(!received_data.first) {
            continue;
        }

        // When using file roll over, write the file format before switching to the next file.
        if (!writer->is_data_for_current_file(received_data.first->frame_index)) {
            #ifdef DEBUG_OUTPUT
                using namespace date;
                cout << "[" << std::chrono::system_clock::now() << "]";
                cout << "[ProcessManager::write_h5] Frame index " << received_data.first->frame_index;
                cout << " does not belong to current file. Write format before the file will be closed." << endl;
            #endif

            writer->write_metadata_to_file();

            write_h5_format(writer->get_h5_file());
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
        auto header_values_type = receiver.get_header_values_type();
        if (header_values_type) {

            for (const auto& header_type : *header_values_type) {

                auto& name = header_type.first;
                auto value = received_data.first->header_values.at(name);

                // TODO: Ugly hack until we get the start sequence in the bsread stream itself.
                if (name == "pulse_id") {
                    if (!last_pulse_id) {
                        last_pulse_id = *(reinterpret_cast<uint64_t*>(value.get()));
                        notify_first_pulse_id(last_pulse_id);
                    } else {
                        last_pulse_id = *(reinterpret_cast<uint64_t*>(value.get()));
                    }
                }

                writer->cache_metadata(name, received_data.first->frame_index, value.get());
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
        
        writer_manager.written_frame(received_data.first->frame_index);
    }

    // Send the last_pulse_id only if it was set.
    if (last_pulse_id) {
        notify_last_pulse_id(last_pulse_id);
    }

    if (writer->is_file_open()) {
        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << std::chrono::system_clock::now() << "]";
            cout << "[ProcessManager::write] Writing file format." << endl;
        #endif

        // Wait until all parameters are set or writer is killed.
        while (!writer_manager.are_all_parameters_set() && !writer_manager.is_killed()) {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(config::parameters_read_retry_interval));
        }

        writer->write_metadata_to_file();
        
        write_h5_format(writer->get_h5_file());
    }
    
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ProcessManager::write] Closing file " << writer_manager.get_output_file() << endl;
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

void ProcessManager::write_h5_format(H5::H5File& file) {

    if (!writer_manager.are_all_parameters_set()) {
        using namespace date;
        std::cout << "[" << std::chrono::system_clock::now() << "]";
        std::cout << "[ProcessManager::write_h5_format] Cannot write file format - REST parameters not set."<< endl;

        return;
    }

    const auto parameters = writer_manager.get_parameters();
    
    try {
        H5FormatUtils::write_format(file, format, parameters);
    } catch (const runtime_error& ex) {
        using namespace date;
        std::cout << "[" << std::chrono::system_clock::now() << "]";
        std::cout << "[ProcessManager::write_h5_format] Error while trying to write file format: "<< ex.what() << endl;
    }
}
