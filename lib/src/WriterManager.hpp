#ifndef WRITERMANAGER_H
#define WRITERMANAGER_H

#include <unordered_map>
#include <string>
#include <atomic>
#include <mutex>
#include <boost/any.hpp>
#include <chrono>
#include <boost/property_tree/json_parser.hpp>
#include "date.h"
#include "H5Format.hpp"

using namespace std;

namespace pt = boost::property_tree;

namespace writer_utils {
    void set_process_id(int user_id);
    void create_destination_folder(const std::string& output_file);
}


class WriterManager
{
    
    std::unordered_map<std::string, boost::any> parameters = {};
    std::mutex parameters_mutex;

    // Initialize in constructor.
    const std::unordered_map<std::string, DATA_TYPE>& parameters_type;
    std::string output_file;
    size_t n_frames;
    std::atomic_bool running_flag;
    std::atomic_bool killed_flag;
    std::atomic<uint64_t> n_received_frames;
    std::atomic<uint64_t> n_written_frames;
    std::atomic<uint64_t> n_lost_frames;

    public:
        WriterManager(const std::unordered_map<std::string, DATA_TYPE>& parameters_type, const std::string& output_file, uint64_t n_frames=0);
        virtual ~WriterManager();

        void stop();
        void kill();
        bool is_running();
        bool is_killed() const;
        std::string get_status();
        bool are_all_parameters_set();
        std::string get_output_file() const;

        const std::unordered_map<std::string, DATA_TYPE>& get_parameters_type() const;
        std::unordered_map<std::string, boost::any> get_parameters();
        void set_parameters(const std::unordered_map<std::string, boost::any>& new_parameters);

        std::unordered_map<std::string, uint64_t> get_statistics() const;
        void received_frame(size_t frame_index);
        void written_frame(size_t frame_index);
        void lost_frame(size_t frame_index);

        size_t get_n_frames() const;

        // statistics variables
        std::tuple<bool, std::string> mode_category;
        uint64_t first_pulse_id;
        int user_id;
        std::chrono::system_clock::time_point time_start;
        std::chrono::system_clock::time_point time_end;
        std::chrono::duration<double> processing_rate;

        // statistics methods
        std::tuple<bool, std::string> get_mode_category() const;
        std::string get_filter() const;
        void set_mode_category(const bool new_mode, const std::string new_category);
        std::string get_writer_stats() const;
        int get_user_id() const;
        size_t get_n_written_frames() const;
        size_t get_n_received_frames() const;
        uint64_t get_n_lost_frames() const;
        void set_processing_rate(std::chrono::duration<double> diff);
        void set_time_end();






};

#endif