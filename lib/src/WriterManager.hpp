#ifndef WRITERMANAGER_H
#define WRITERMANAGER_H

#include <unordered_map>
#include <string>
#include <atomic>
#include <mutex>
#include <boost/any.hpp>
#include <chrono>
#include "date.h"
#include <deque>
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
    const std::deque<WriterManagerLog> logs;

    std::atomic<bool> running_flag;

    std::atomic<bool> writing_flag;
    std::atomic<bool> receiving_flag;

    std::atomic<int64_t> n_frames_to_receive;
    std::atomic<int64_t> n_frames_to_write;

    public:
        WriterManager();
        virtual ~WriterManager();

        void start(std::string output_file, int n_frames, int user_id);
        void stop();

        std::string get_status();
        std::unordered_map<std::string, uint64_t> get_statistics() const;

        // Return True if the frame is to be received, False if is to be dropped.
        bool receive_frame();
        // True if the process should conitnue.
        bool is_running() const;

        // Return True if the frame is to be written, False otherwise.
        bool write_frame();
        // True if the writing should continue.
        bool is_writing() const;
        
        
        // Signal that the writing has completed.
        void writing_completed();
        void writing_error(std::string error_message);
};

#endif
