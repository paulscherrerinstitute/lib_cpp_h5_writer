#ifndef BUFFER-UTILS_HPP
#define BUFFER-UTILS_HPP

const size_t FILE_MOD = 1000;
const size_t FOLDER_MOD = 100000;

std::string get_filename(
        std::string root_folder,
        std::string device_name,
        uint64_t pulse_id)
{
    uint64_t folder_base = pulse_id / FOLDER_MOD;
    folder_base *= FOLDER_MOD;

    uint64_t file_base = pulse_id / FILE_MOD;
    file_base *= FILE_MOD;

    std::stringstream folder;
    folder << root_folder << "/";
    folder << device_name << "/";
    folder << folder_base << "/";
    folder << file_base << ".h5";

    return folder.str();
}

std::size_t get_file_frame_index(uint64_t pulse_id)
{
    uint64_t file_base = pulse_id / FILE_MOD;
    file_base *= FILE_MOD;

    return pulse_id - file_base;
}

#endif //BUFFER-UTILS_HPP
