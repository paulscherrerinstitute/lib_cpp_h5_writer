#ifndef WRITERMANAGER_H
#define WRITERMANAGER_H

#include <unordered_map>
#include <string>
#include <atomic>
#include <mutex>
#include <boost/any.hpp>
#include <boost/thread.hpp>
#include <chrono>
#include "date.h"
#include <deque>

#include "ZmqReceiver.hpp"
#include "RingBuffer.hpp"
#include "H5Format.hpp"

namespace writer_utils {
    void set_process_id(int user_id);
    void create_destination_folder(const std::string& output_file);
}

struct WriterManagerLog
{
    std::string filename;
    std::string status;

    uint64_t n_requested_frames;
    uint64_t n_received_frames;
    uint64_t n_written_frames;
};


class WriterManager
{
    // Initialize in constructor.

    std::atomic<bool> running_flag;

    std::atomic<bool> writing_flag;
    std::atomic<bool> receiving_flag;

    std::atomic<int64_t> n_frames_to_receive;
    std::atomic<int64_t> n_frames_to_write;


    protected:
        RingBuffer& ring_buffer;
        const H5Format& format;
        hsize_t frames_per_file;

        boost::thread writing_thread;

        typedef std::unordered_map<std::string, HeaderDataType> header_map;
        std::shared_ptr<header_map> header_values_type = NULL;

        const std::deque<WriterManagerLog> logs;

        void write_h5(std::string output_file, 
                      uint64_t n_frames);
        void write_h5_format(H5::H5File& file);


    public:
        WriterManager(RingBuffer& ring_buffer, 
                      const H5Format& format, 
                      std::shared_ptr<header_map> header_values_type,
                      hsize_t frames_per_file=0);

        virtual ~WriterManager();

        void start(std::string output_file, int n_frames, int user_id);
        void stop();

        std::string get_status();
        std::unordered_map<std::string, uint64_t> get_statistics() const;

        // Return True if the frame is to be received, False if is to be dropped.
        bool receive_frame();
        // True if the process should continue.
        bool is_running() const;
        bool is_writing() const;

        // Return True if the frame is to be written, False otherwise.
        bool write_frame();
        // True if the writing should continue.
        
        // Signal that the writing has completed.
        void writing_completed();
        void writing_error(std::string error);
};

#endif
