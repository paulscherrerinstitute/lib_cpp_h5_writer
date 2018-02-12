#ifndef WRITERMANAGER_H
#define WRITERMANAGER_H

#include <map>
#include <string>
#include <atomic>
#include <mutex>
#include <boost/any.hpp>

#include "H5Format.hpp"

class WriterManager
{
    
    std::map<std::string, boost::any> parameters = {};
    std::mutex parameters_mutex;

    // Initialize in constructor.
    const std::map<std::string, DATA_TYPE>& parameters_type;
    std::string output_file;
    size_t n_frames;
    std::atomic_bool running_flag;
    std::atomic_bool killed_flag;
    std::atomic<uint64_t> n_received_frames;
    std::atomic<uint64_t> n_written_frames;
    std::atomic<uint64_t> n_lost_frames;

    public:
        WriterManager(const std::map<std::string, DATA_TYPE>& parameters_type, const std::string& output_file, uint64_t n_frames=0);
        virtual ~WriterManager();

        void stop();
        void kill();
        bool is_running();
        bool is_killed() const;
        std::string get_status();
        bool are_all_parameters_set();
        std::string get_output_file() const;

        const std::map<std::string, DATA_TYPE>& get_parameters_type() const;
        std::map<std::string, boost::any> get_parameters();
        void set_parameters(const std::map<std::string, boost::any>& new_parameters);
        
        std::map<std::string, uint64_t> get_statistics() const;
        void received_frame(size_t frame_index);
        void written_frame(size_t frame_index);
        void lost_frame(size_t frame_index);
};

#endif