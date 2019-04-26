#include <iostream>
#include <sstream>

#include <boost/thread.hpp>
#include "WriterManager.hpp"
#include "MetadataBuffer.hpp"
#include "BufferedWriter.hpp"
#include "config.hpp"

using namespace std;

void writer_utils::set_process_id(int user_id)
{

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono; 
        error_message << "[" << system_clock::now() << "]";
        cout << "[writer_utils::set_process_id] Setting process user to ";
        cout << user_id << endl;
    #endif

    if (setegid(user_id)) {
        stringstream error_message;

        using namespace date;
        using namespace chrono; 
        error_message << "[" << system_clock::now() << "]";
        error_message << "[writer_utils::set_process_id] Cannot set group_id to ";
        error_message << user_id << endl;

        throw runtime_error(error_message.str());
    }

    if (seteuid(user_id)) {
        stringstream error_message;
        using namespace date;
        using namespace chrono; 
        error_message << "[" << system_clock::now() << "]";
        error_message << "[writer_utils::set_process_id] Cannot set user_id to ";
        error_message << user_id << endl;

        throw runtime_error(error_message.str());
    }
}

void writer_utils::create_destination_folder(const string& output_file)
{
    auto file_separator_index = output_file.rfind('/');

    if (file_separator_index != string::npos) {
        string output_folder(output_file.substr(0, file_separator_index));

        using namespace date;
        using namespace chrono; 
        cout << "[" << system_clock::now() << "]";
        cout << "[writer_utils::create_destination_folder] Creating folder ";
        cout << output_folder << endl;

        string create_folder_command("mkdir -p " + output_folder);
        system(create_folder_command.c_str());
    }
}

WriterManager::WriterManager(
    RingBuffer& ring_buffer, 
    const H5Format& format, 
    std::shared_ptr<header_map> header_values_type,
    hsize_t frames_per_file):
        ring_buffer(ring_buffer),
        format(format),
        header_values_type(header_values_type),
        logs(10)
{
    running_flag = true; 

    writing_flag = false;
    receiving_flag = false;

    n_frames_to_receive = 0;
    n_frames_to_write = 0;

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono; 
        error_message << "[" << system_clock::now() << "]";
        cout << "[WriterManager::WriterManager] Writer manager initialized." << endl;
    #endif
}

WriterManager::~WriterManager(){}

void WriterManager::stop()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[WriterManager::stop] Stopping the writer." << endl;
    #endif
    
    running_flag = false;
    
    
}

string WriterManager::get_status()
{
    if (writing_flag) {
        return "writing";
    } else if (running_flag) {
        return "ready";
    } else {
        return "Error.. I guess. This shouldn't be possible?";
    }
}

unordered_map<string, uint64_t> WriterManager::get_statistics() const
{
    unordered_map<string, uint64_t> result = {
        {"n_frames_receive", n_frames_to_receive.load()},
        {"n_frames_to_write", n_frames_to_write.load()}
    };

    return result;
}


void WriterManager::start(const string output_file,
                          const int n_frames, 
                          const int user_id)
{

    #ifdef DEBUG_OUTPUT
        stringstream output_message;
        using namespace date;
        output_message << "[" << std::chrono::system_clock::now() << "]";
        output_message << "[WriterManager::start] Starting with parameters: ";

        for (const auto& parameter : new_parameters) {
            auto& parameter_name = parameter.first;
            auto& parameter_value = parameter.second;

            output_message << parameter_name << ": " << parameter_value << ", ";
        }

        cout << output_message.str() << endl;
    #endif

    n_frames_to_write = n_frames;
    writing_flag = true;

    n_frames_to_receive = n_frames;
    receiving_flag = true;

    
    writing_thread = boost::thread(&WriterManager::write_h5, 
                                   this, 
                                   output_file, 
                                   n_frames);

    //TODO: Sent this event somewhere?
}

bool WriterManager::is_running() const
{
    return running_flag.load();
}

bool WriterManager::is_writing() const
{
    return writing_flag.load();
}

bool WriterManager::receive_frame() {
    if (n_frames_to_receive > 0) {
        return (n_frames_to_receive.fetch_sub(1) >= 0);
    }

    return false;
}

bool WriterManager::write_frame() {
    if (n_frames_to_write > 0) {
        return (n_frames_to_write.fetch_sub(1) >= 0);
    }

    return false;
}

void WriterManager::writing_completed() {
    writing_flag = false;    

    #ifdef DEBUG_OUTPUT
        stringstream output_message;
        using namespace date;
        output_message << "[" << std::chrono::system_clock::now() << "]";
        output_message << "[WriterManager::writing_completed] Writing has finished.";
        output_message << endl;
    #endif

    //TODO: Send this event somewhere somehow?
}

void WriterManager::writing_error(string error) {

}

void WriterManager::write_h5_format(H5::H5File& file) {

    try {
        H5FormatUtils::write_format(file, format, {});
    } catch (const runtime_error& ex) {
        using namespace date;
        using namespace chrono;

        cout << "[" << system_clock::now() << "]";
        cout << "[ProcessManager::write_h5_format] Error while";
        cout << " trying to write file format: "<< ex.what() << endl;
    }
}

void WriterManager::write_h5(string output_file, uint64_t n_frames)
{
    try {

        size_t metadata_buffer_size = 
            frames_per_file != 0 ? frames_per_file : n_frames;

        auto metadata_buffer = unique_ptr<MetadataBuffer>(
            new MetadataBuffer(metadata_buffer_size, 
                               header_values_type));
    
        auto writer = get_buffered_writer(
            output_file, 
            n_frames, 
            move(metadata_buffer), 
            frames_per_file, 
            config::dataset_increase_step);
    
        writer->create_file();
            
        auto raw_frames_dataset_name = config::raw_image_dataset_name;
    
        uint64_t last_pulse_id = 0;
        
        while(is_writing() || !ring_buffer.is_empty()) {
            
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
            if (!write_frame()) {
                break;
            }
    
            // Write file format before rolling to next file.
            if (!writer->is_data_for_current_file(
                    received_data.first->frame_index)) {
                
                #ifdef DEBUG_OUTPUT
                    using namespace date;
                    using namespace chrono;

                    cout << "[" << system_clock::now() << "]";
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
                using namespace chrono;

                auto start_time_frame = system_clock::now();
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
    
                auto frame_time_difference = system_clock::now() - start_time_frame;

                auto frame_diff_ms = 
                    duration<float, milli>(frame_time_difference).count();
    
                cout << "[" << system_clock::now() << "]";
                cout << "[PSIWriter::write_h5] Frame index "; 
                cout << received_data.first->frame_index;
                cout << " written in " << frame_diff_ms << " ms." << endl;
            #endif
    
            ring_buffer.release(received_data.first->buffer_slot_index);
    
            #ifdef PERF_OUTPUT
                using namespace date;
                using namespace chrono;

                auto start_time_metadata = system_clock::now();
            #endif
    
            // Write image metadata if mapping specified.
            if (header_values_type) {
    
                for (const auto& header_type : *header_values_type) {
    
                    auto& name = header_type.first;
                    auto value = received_data.first->header_values.at(name);
    
                    // TODO: Ugly hack until we get the start sequence in bsread.
                    if (name == "pulse_id") {
                        if (!last_pulse_id) {
                            last_pulse_id = *(reinterpret_cast<uint64_t*>(value.get()));
                            //notify_first_pulse_id(last_pulse_id);
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
    
                auto metadata_time_difference = system_clock::now() - start_time_metadata;
                auto metadata_diff_ms = duration<float, milli>(metadata_time_difference).count();
    
                cout << "[" << system_clock::now() << "]";
                cout << "[ProcessManager::write_h5] Frame metadata index "; 
                cout << received_data.first->frame_index << " written in " << metadata_diff_ms << " ms." << endl;
            #endif
        }
    
        // Send the last_pulse_id only if it was set.
        if (last_pulse_id) {
            //notify_last_pulse_id(last_pulse_id);
        }
    
        if (writer->is_file_open()) {
            #ifdef DEBUG_OUTPUT
                using namespace date;
                using namespace chrono;

                cout << "[" << system_clock::now() << "]";
                cout << "[ProcessManager::write] Writing file format." << endl;
            #endif
    
            writer->write_metadata_to_file();
            
            write_h5_format(writer->get_h5_file());
        }
        
        #ifdef DEBUG_OUTPUT
            using namespace date;
            using namespace chrono;

            cout << "[" << system_clock::now() << "]";
            cout << "[ProcessManager::write] Closing file " << get_output_file() << endl;
        #endif
        
        writer->close_file(); 
    
        #ifdef DEBUG_OUTPUT
            using namespace date;
            using namespace chrono;
            
            cout << "[" << system_clock::now() << "]";
            cout << "[ProcessManager::write] Writer thread stopped." << endl;
        #endif
    
        writing_completed();

    } catch (const exception& ex) {
        writing_error(ex.what());
    }
}

