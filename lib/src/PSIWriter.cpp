#include "PSIWriter.hpp"


void PSIWriter::notify_first_pulse_id(uint64_t pulse_id) 
{
    string request_address(bsread_rest_address);

    async(launch::async, [pulse_id, &request_address]{
        try {

            cout << "Sending first received pulse_id " << pulse_id <<;
            cout << " to bsread_rest_address " << request_address << endl;

            stringstream request;
            request << "curl -X PUT " << request_address;
            request << "/start_pulse_id/" << pulse_id;

            string request_call(request.str());

            #ifdef DEBUG_OUTPUT
                using namespace date;
                cout << "[" << chrono::system_clock::now() << "]";
                cout << "[ProcessManager::notify_first_pulse_id] Sending request";
                cout << "(" << request_call << ")." << endl;
            #endif

            system(request_call.c_str());
        } catch (...){}
        
    });
}


void PSIWriter::notify_last_pulse_id(uint64_t pulse_id) 
{

    try {
        cout << "Sending last received pulse_id " << pulse_id;
        cout << " to bsread address " << bsread_rest_address << endl;

        stringstream request;
        request << "curl -X PUT " << bsread_rest_address;
        request << "/stop_pulse_id/" << pulse_id;

        cout << "Request: " << request.str() << endl;

        string request_call(request.str());

        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << chrono::system_clock::now() << "]";
            cout << "[ProcessManager::notify_last_pulse_id] Sending request";
            cout << "(" << request_call << ")." << endl;
        #endif

        system(request_call.c_str());
    } catch (...){}
}


void PSIWriter::run_writer(std::string output_file, uint64_t n_frames)
{
    

}

void PSIWriter::write_h5 (string output_file, uint64_t n_frames)
{
    try {

        size_t metadata_buffer_size = 
            frames_per_file != 0 ? frames_per_file : n_frames;

        auto metadata_buffer = unique_ptr<MetadataBuffer>(
            new MetadataBuffer(metadata_buffer_size, 
                               receiver.get_header_values_type()));
    
        auto writer = get_buffered_writer(
            output_file, 
            n_frames, 
            move(metadata_buffer), 
            frames_per_file, 
            config::dataset_increase_step);
    
        writer->create_file();
            
        auto raw_frames_dataset_name = config::raw_image_dataset_name;
    
        uint64_t last_pulse_id = 0;
        
        while(writer_manager.is_writing() || !ring_buffer.is_empty()) {
            
            if (ring_buffer.is_empty()) {
                boost::this_thread::sleep_for(
                    boost::chrono::milliseconds(
                        config::ring_buffer_read_retry_interval));
                continue;
            }
    
            const pair< shared_ptr<FrameMetadata>, char* > received_data =
                ring_buffer.read();
            
            // NULL pointer means that the ringbuffer->read() timeouted.
            if(!received_data.first) {
                continue;
            }
    
            // The acquisition stops when there are no more frames to write.
            if (!writer_manager.write_frame()) {
                break;
            }
    
            // Write file format before rolling to next file.
            if (!writer->is_data_for_current_file(
                    received_data.first->frame_index)) {
                
                #ifdef DEBUG_OUTPUT
                    using namespace date;
                    cout << "[" << chrono::system_clock::now() << "]";
                    cout << "[PSIWriter::write_h5] Frame index ";
                    cout << received_data.first->frame_index;
                    cout << " does not belong to current file. ";
                    cout << " Write format before switching file." << endl;
                #endif
    
                writer->write_metadata_to_file();
    
                write_h5_format(writer->get_h5_file());
            }
    
            #ifdef PERF_OUTPUT
                using namespace date;
                auto start_time_frame = chrono::system_clock::now();
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
                using namespace chrono;
    
                auto frame_time_difference = 
                    chrono::system_clock::now() - start_time_frame;

                auto frame_diff_ms = 
                    duration<float, milli>(frame_time_difference).count();
    
                cout << "[" << chrono::system_clock::now() << "]";
                cout << "[PSIWriter::write_h5] Frame index "; 
                cout << received_data.first->frame_index;
                cout << " written in " << frame_diff_ms << " ms." << endl;
            #endif
    
            ring_buffer.release(received_data.first->buffer_slot_index);
    
            #ifdef PERF_OUTPUT
                using namespace date;
                auto start_time_metadata = chrono::system_clock::now();
            #endif
    
            // Write image metadata if mapping specified.
            auto header_values_type = receiver.get_header_values_type();
            if (header_values_type) {
    
                for (const auto& header_type : *header_values_type) {
    
                    auto& name = header_type.first;
                    auto value = received_data.first->header_values.at(name);
    
                    // TODO: Ugly hack until we get the start sequence in bsread.
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
                using namespace chrono;
    
                auto metadata_time_difference = chrono::system_clock::now() - start_time_metadata;
                auto metadata_diff_ms = duration<float, milli>(metadata_time_difference).count();
    
                cout << "[" << chrono::system_clock::now() << "]";
                cout << "[ProcessManager::write_h5] Frame metadata index "; 
                cout << received_data.first->frame_index << " written in " << metadata_diff_ms << " ms." << endl;
            #endif
        }
    
        // Send the last_pulse_id only if it was set.
        if (last_pulse_id) {
            notify_last_pulse_id(last_pulse_id);
        }
    
        if (writer->is_file_open()) {
            #ifdef DEBUG_OUTPUT
                using namespace date;
                cout << "[" << chrono::system_clock::now() << "]";
                cout << "[ProcessManager::write] Writing file format." << endl;
            #endif
    
            writer->write_metadata_to_file();
            
            write_h5_format(writer->get_h5_file());
        }
        
        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << chrono::system_clock::now() << "]";
            cout << "[ProcessManager::write] Closing file " << writer_manager.get_output_file() << endl;
        #endif
        
        writer->close_file(); 
    
        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << chrono::system_clock::now() << "]";
            cout << "[ProcessManager::write] Writer thread stopped." << endl;
        #endif
    
        writer_manager.writing_completed();

    } catch (const exception& ex) {
        writer_manager.writing_error(ex.what());
    }
}

void PSIWriter::write_h5_format(H5::H5File& file) {

    const auto parameters = writer_manager.get_parameters();
    
    try {
        H5FormatUtils::write_format(file, format, parameters);
    } catch (const runtime_error& ex) {
        using namespace date;
        cout << "[" << chrono::system_clock::now() << "]";
        cout << "[ProcessManager::write_h5_format] Error while";
        cout << " trying to write file format: "<< ex.what() << endl;
    }
}
