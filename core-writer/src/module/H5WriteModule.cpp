#include "H5WriteModule.hpp"
#include <iostream>
#include <MetadataBuffer.hpp>
#include "BufferedWriter.hpp"

using namespace std;

H5WriteModule::H5WriteModule(
        RingBuffer& ring_buffer,
        const header_map& header_values,
        const H5Format& format) :
            ring_buffer_(ring_buffer),
            header_values_(header_values),
            format_(format),
            is_writing_(false)
{
}

void H5WriteModule::start_writing(
        const string& output_file,
        const int n_frames,
        const int user_id)
{
    if (is_writing_ == true) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono;
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[H5WriteModule::start_writing]";
        err_msg << " Writer already running." << endl;

        throw runtime_error(err_msg.str());
    }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[H5WriteModule::start_writing]";
        cout << " Start writing with parameters:" << endl;
        cout << "\toutput_file: " << output_file;
        cout << "\tn_frames: " << n_frames;
        cout << "\tuser_id: " << user_id;
        cout << endl;
    #endif

   is_writing_ = true;

   writing_thread_ = thread(
           &H5WriteModule::write_thread, this,
           output_file,
           n_frames,
           user_id);
}

void H5WriteModule::stop_writing()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[H5WriteModule::stop_writing]";
        cout << " Disable writing." << endl;
    #endif

    is_writing_ = false;

    if (writing_thread_.joinable()) {
        writing_thread_.join();
    }
}

void H5WriteModule::write_thread(
        const std::string& output_file,
        const int n_frames,
        const int user_id)
{
    // TODO: Take into account file rollover.
    size_t metadata_buffer_length = n_frames;
    MetadataBuffer metadata_buffer(metadata_buffer_length, header_values_);

    BufferedWriter writer(output_file, n_frames, metadata_buffer);

    auto raw_frames_dataset_name = config::raw_image_dataset_name;

    writer.create_file();
    uint64_t last_pulse_id = 0;

    while(is_writing_.load(memory_order_relaxed)) {

        auto received_data = ring_buffer_.read();

        // .first is nullptr if ringbuffer is empty.
        if(received_data.first == nullptr) {
            this_thread::sleep_for(chrono::milliseconds(
                    config::ring_buffer_read_retry_interval));
            continue;
        }

        // Write file format before rolling to next file.
        if (!writer.is_data_for_current_file(
                received_data.first->frame_index)) {

                #ifdef DEBUG_OUTPUT
                    using namespace date;
                    using namespace chrono;

                    cout << "[" << system_clock::now() << "]";
                    cout << "[H5WriteModule::write_thread] Frame index ";
                    cout << received_data.first->frame_index;
                    cout << " does not belong to current file. ";
                    cout << " Write format before switching file." << endl;
                #endif

                writer.write_metadata_to_file();
                H5FormatUtils::write_format(writer.get_h5_file(), format_, {});
        }

        #ifdef PERF_OUTPUT
            using namespace date;
            using namespace chrono;

            auto start_time_frame = system_clock::now();
        #endif

        // Write image data.
        writer.write_data(
                raw_frames_dataset_name,
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
            cout << "[H5WriteModule::write_thread] Frame index ";
            cout << received_data.first->frame_index;
            cout << " written in " << frame_diff_ms << " ms." << endl;
        #endif

        ring_buffer_.release(received_data.first->buffer_slot_index);

        #ifdef PERF_OUTPUT
            using namespace date;
            using namespace chrono;

            auto start_time_metadata = system_clock::now();
        #endif

        for (const auto& header_type : header_values_) {

            auto& name = header_type.first;
            auto value = received_data.first->header_values.at(name);

            writer.cache_metadata(
                    name,
                    received_data.first->frame_index,
                    value.get());
        }

        #ifdef PERF_OUTPUT
            using namespace date;
            using namespace chrono;

            auto metadata_time_difference = system_clock::now() - start_time_metadata;
            auto metadata_diff_ms = duration<float, milli>(metadata_time_difference).count();

            cout << "[" << system_clock::now() << "]";
            cout << "[H5WriteModule::write_thread] Frame metadata index ";
            cout << received_data.first->frame_index << " written in " << metadata_diff_ms << " ms." << endl;
        #endif
    }

    if (writer.is_file_open()) {
        #ifdef DEBUG_OUTPUT
            using namespace date;
            using namespace chrono;

            cout << "[" << system_clock::now() << "]";
            cout << "[H5WriteModule::write_thread]";
            cout << " Writing file format." << endl;
        #endif

        writer.write_metadata_to_file();
        H5FormatUtils::write_format(writer.get_h5_file(), format_, {});

        writer.close_file();
    }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;

        cout << "[" << system_clock::now() << "]";
        cout << "[H5WriteModule::write_thread]";
        cout << " Writer thread stopped." << endl;
    #endif
}
