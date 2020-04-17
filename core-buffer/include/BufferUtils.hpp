#ifndef BUFFER_UTILS_HPP
#define BUFFER_UTILS_HPP

#include <string>

namespace BufferUtils
{
    extern const size_t FILE_MOD;
    extern const size_t FOLDER_MOD;

    std::string get_filename(
            std::string root_folder,
            std::string device_name,
            uint64_t pulse_id);

    std::size_t get_file_frame_index(uint64_t pulse_id);
}

#endif //BUFFER_UTILS_HPP
