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
        cout << "[" << system_clock::now() << "]";
        cout << "[writer_utils::set_process_id]";
        cout << " Setting process user to " << user_id << endl;
    #endif

    if (setegid(user_id)) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono;
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[writer_utils::set_process_id]";
        err_msg << " Cannot set group_id to " << user_id << endl;

        throw runtime_error(err_msg.str());
    }

    if (seteuid(user_id)) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono;
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[writer_utils::set_process_id]";
        err_msg << " Cannot set user_id to " << user_id << endl;

        throw runtime_error(err_msg.str());
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
        cout << "[writer_utils::create_destination_folder]";
        cout << " Creating folder " << output_folder << endl;

        string create_folder_command("mkdir -p " + output_folder);
        system(create_folder_command.c_str());
    }
}

WriterManager::WriterManager(
    ZmqReceiver& receiver,
    RingBuffer& ring_buffer, 
    const H5Format& format, 
    std::shared_ptr<header_map> header_values_type,
    uint8_t n_receiving_threads,
    hsize_t frames_per_file):
        receiver(receiver),
        ring_buffer(ring_buffer),
        format(format),
        header_values_type(header_values_type),
        n_receiving_threads(n_receiving_threads),
        frames_per_file(frames_per_file),
        receiving_threads(frames_per_file),
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
        cout << "[" << system_clock::now() << "]";
        cout << "[WriterManager::WriterManager]";
        cout << " Writer manager initialized." << endl;
    #endif
}

WriterManager::~WriterManager()
{
    stop();
}

void WriterManager::stop()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[WriterManager::stop]";
        cout << " Stopping the writer." << endl;
    #endif
    
    running_flag = false;
  
    for (auto& thread_ptr:receiving_threads) {
        thread_ptr->join();
    }
    receiving_threads.clear();
    
    writing_thread.join(); 
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
        using namespace date;
        using namespace chrono; 
        cout << "[" << system_clock::now() << "]";
        cout << "[WriterManager::start]";
        cout << " Starting with parameters:";
        cout << "\toutput_file: " << output_file;
        cout << "\tn_frames: " << n_frames;
        cout << "\tuser_id: " << user_id;
        cout << endl;
    #endif

    n_frames_to_write = n_frames;
    writing_flag = true;

    n_frames_to_receive = n_frames;
    receiving_flag = true;

    writing_thread = boost::thread(&WriterManager::write_h5,
                                   this, 
                                   output_file, 
                                   n_frames);
    
    for (uint8_t i_rec=0; i_rec < n_receiving_threads; i_rec++) {
        receiving_threads.push_back(unique_ptr<boost::thread>(
            new boost::thread(&WriterManager::receive_zmq, this)
        ));
    }

}


bool WriterManager::is_running() const
{
    return running_flag.load();
}

bool WriterManager::is_writing() const
{
    return writing_flag.load();
}

bool WriterManager::should_receive_frame() {
    if (n_frames_to_receive > 0) {
        return (n_frames_to_receive.fetch_sub(1) >= 0);
    }

    return false;
}

bool WriterManager::should_write_frame() {
    if (n_frames_to_write > 0) {
        return (n_frames_to_write.fetch_sub(1) >= 0);
    }

    return false;
}

void WriterManager::writing_completed() {
    writing_flag = false;    

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono; 
        cout << "[" << system_clock::now() << "]";
        cout << "[WriterManager::writing_completed]";
        cout << " Writing has finished." << endl;
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

void WriterManager::write_h5(const string output_file, const uint64_t n_frames)
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
            if (!should_write_frame()) {
                break;
            }
    
            // Write file format before rolling to next file.
            if (!writer->is_data_for_current_file(
                    received_data.first->frame_index)) {
                
                #ifdef DEBUG_OUTPUT
                    using namespace date;
                    using namespace chrono;

                    cout << "[" << system_clock::now() << "]";
                    cout << "[WriterManager::write_h5] Frame index ";
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
                cout << "[WriterManager::write_h5] Frame index "; 
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
    
                    writer->cache_metadata(
                            name,
                            received_data.first->frame_index,
                            value.get());
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
    
        if (writer->is_file_open()) {
            #ifdef DEBUG_OUTPUT
                using namespace date;
                using namespace chrono;

                cout << "[" << system_clock::now() << "]";
                cout << "[ProcessManager::write]";
                cout << " Writing file format." << endl;
            #endif
    
            writer->write_metadata_to_file();
            
            write_h5_format(writer->get_h5_file());
        }
        
        #ifdef DEBUG_OUTPUT
            using namespace date;
            using namespace chrono;

            cout << "[" << system_clock::now() << "]";
            cout << "[ProcessManager::write]";
            cout << " Closing file " << output_file << endl;
        #endif
        
        writer->close_file(); 
    
        #ifdef DEBUG_OUTPUT
            using namespace date;
            using namespace chrono;
            
            cout << "[" << system_clock::now() << "]";
            cout << "[ProcessManager::write]";
            cout << " Writer thread stopped." << endl;
        #endif
    
        writing_completed();

    } catch (const exception& ex) {
        writing_error(ex.what());
    }
}

void WriterManager::receive_zmq()
{
    receiver.connect();

    while (is_running()) {
        
        auto frame = receiver.receive();
        
        // If receive timeout, both pointers are NULL.
        if (!frame.first || !receive_frame()){
            continue;
        }

        auto frame_metadata = frame.first;
        auto frame_data = frame.second;

        #ifdef DEBUG_OUTPUT
            using namespace date;
            using namespace chrono;
            cout << "[" << system_clock::now() << "]";
            cout << "[ProcessManager::receive_zmq]";
            cout << " Processing FrameMetadata";
            cout << " with frame_index " << frame_metadata->frame_index;
            cout << " and frame_shape [" << frame_metadata->frame_shape[0];
            cout << ", " << frame_metadata->frame_shape[1] << "]";
            cout << " and endianness " << frame_metadata->endianness;
            cout << " and type " << frame_metadata->type;
            cout << " and frame_bytes_size "; 
            cout << frame_metadata->frame_bytes_size << "." << endl;
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
            using namespace chrono;
            cout << "[" << system_clock::now() << "]";
            cout << "[WriterManager::receive_zmq]";
            cout << " Compressed image from ";
            cout << frame_metadata->frame_bytes_size << " bytes to "; 
            cout << compressed_size << " bytes." << endl; 
        #endif

        frame_metadata->frame_bytes_size = compressed_size;

        ring_buffer.commit(frame_metadata);
   }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[WriterManager::receive_zmq]";
        cout << " Receiver thread stopped." << endl;
    #endif
}
