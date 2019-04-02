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

    uint64_t n_requested_frames;
    uint64_t n_received_frames;
    uint64_t n_written_frames;
};


class WriterManager
{
    // Initialize in constructor.
    const std::unordered_map<std::string, DATA_TYPE>& parameters_type;
    const std::deque<WriterManagerLog> logs;

    std::atomic<bool> receiving_flag;
    std::atomic<bool> writing_flag;
    std::atomic<bool> killed_flag;

    std::atomic<uint64_t> n_received_frames;
    std::atomic<uint64_t> n_written_frames;
    std::atomic<int64_t> n_expected_frames;

    public:
        WriterManager(const std::unordered_map<std::string, DATA_TYPE>& parameters_type);
        virtual ~WriterManager();

        void start(const std::unordered_map<std::string, boost::any>& new_parameters);
        void stop();
        void kill();
        std::string get_status();
        std::unordered_map<std::string, uint64_t> get_statistics() const;

        std::unordered_map<std::string, boost::any> get_parameters();
        const std::unordered_map<std::string, DATA_TYPE>& get_parameters_type() const;

        bool is_running();
        bool is_killed() const;
        bool are_all_parameters_set();
        std::string get_output_file() const;


        void received_frame(size_t frame_index);
        void written_frame(size_t frame_index);
        void lost_frame(size_t frame_index);
};

#endif
