#ifndef BUFFER_UTILS_HPP
#define BUFFER_UTILS_HPP

#include <string>
#include <vector>

namespace BufferUtils
{
    extern const size_t FILE_MOD;
    extern const size_t FOLDER_MOD;
    extern const std::string FILE_EXTENSION;

    std::string get_filename(
            std::string root_folder,
            std::string device_name,
            uint64_t pulse_id);

    std::size_t get_file_frame_index(uint64_t pulse_id);

    void update_latest_file(
            const std::string& latest_filename,
            const std::string& filename_to_write);

    std::string get_latest_file(const std::string& latest_filename);

    struct path_sufix {
        uint64_t start_pulse_id;
        uint64_t stop_pulse_id;
        std::string path;
    };

    std::vector<path_sufix> get_path_suffixes(
            const uint64_t start_pulse_id,
            const uint64_t stop_pulse_id);
}

#endif //BUFFER_UTILS_HPP
