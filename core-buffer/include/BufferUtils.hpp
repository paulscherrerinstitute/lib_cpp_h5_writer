#ifndef BUFFER_UTILS_HPP
#define BUFFER_UTILS_HPP

#include <string>

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
}

#endif //BUFFER_UTILS_HPP
