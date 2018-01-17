#ifndef WRITERMANAGER_H
#define WRITERMANAGER_H

#include <map>
#include <string>
#include <atomic>
#include <boost/any.hpp>
#include "config.hpp"
#include "h5_utils.hpp"

class WriterManager
{
    std::map<std::string, boost::any> parameters = {};

    // Initialize in constructor.
    size_t n_images;
    std::atomic_bool running_flag;
    std::atomic_int n_received_frames;
    std::atomic_int n_written_frames;

    public:
        WriterManager(uint64_t n_images=0, std::string dataset_name=config::dataset_name);
        void stop();
        std::string get_status();
        std::map<std::string, uint64_t> get_statistics();
        std::map<std::string, boost::any>& get_parameters();
        void set_parameters(std::map<std::string, boost::any>& new_parameters);
        bool is_running();
        void received_frame(size_t frame_index);
        void written_frame(size_t frame_index);
};

#endif