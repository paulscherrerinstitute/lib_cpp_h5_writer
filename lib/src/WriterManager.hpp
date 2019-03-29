#ifndef WRITERMANAGER_H
#define WRITERMANAGER_H

#include <unordered_map>
#include <string>
#include <atomic>
#include <mutex>
#include <boost/any.hpp>
#include <chrono>
#include "date.h"

#include "H5Format.hpp"

namespace writer_utils {
    void set_process_id(int user_id);
    void create_destination_folder(const std::string& output_file);
}


class WriterManager
{
    
    std::unordered_map<std::string, boost::any> parameters = {};

    // Initialize in constructor.
    const std::unordered_map<std::string, DATA_TYPE>& parameters_type;
    std::atomic_bool running_flag;
    std::atomic_bool killed_flag;
    std::atomic<uint64_t> n_received_frames;
    std::atomic<uint64_t> n_written_frames;
    std::atomic<uint64_t> n_lost_frames;

    public:
        WriterManager(const std::unordered_map<std::string, DATA_TYPE>& parameters_type);
        virtual ~WriterManager();

        void start(const std::unordered_map<std::string, boost:any>& parameters);
        const std::unordered_map<std::string, DATA_TYPE>& get_parameters_type() const;
        void stop();
        void kill();
        std::string get_status();
        std::unordered_map<std::string, uint64_t> get_statistics() const;

        bool is_running();
        bool is_killed() const;
        bool are_all_parameters_set();
        std::string get_output_file() const;

        std::unordered_map<std::string, boost::any> get_parameters();
        
        void received_frame(size_t frame_index);
        void written_frame(size_t frame_index);
        void lost_frame(size_t frame_index);
};

#endif
