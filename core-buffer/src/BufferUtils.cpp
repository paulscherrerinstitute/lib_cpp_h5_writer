using namespace std;

#include "BufferUtils.hpp"
#include <sstream>

const size_t BufferUtils::FILE_MOD = 1000;
const size_t BufferUtils::FOLDER_MOD = 100000;

string BufferUtils::get_filename(
        std::string root_folder,
        std::string device_name,
        uint64_t pulse_id)
{
    uint64_t folder_base = pulse_id / FOLDER_MOD;
    folder_base *= FOLDER_MOD;

    uint64_t file_base = pulse_id / FILE_MOD;
    file_base *= FILE_MOD;

    stringstream folder;
    folder << root_folder << "/";
    folder << device_name << "/";
    folder << folder_base << "/";
    folder << file_base << ".bin";

    return folder.str();
}

size_t BufferUtils::get_file_frame_index(uint64_t pulse_id)
{
    uint64_t file_base = pulse_id / FILE_MOD;
    file_base *= FILE_MOD;

    return pulse_id - file_base;
}

void BufferUtils::update_latest_file(
        const std::string& latest_filename,
        const std::string& filename_to_write)
{
    // TODO: Ugly hack, please please fix it.
    // TODO: This for now works only if the root_folder is absolute path.

    stringstream latest_command;
    latest_command << "echo " << filename_to_write;
    latest_command << " > " << latest_filename;
    auto str_latest_command = latest_command.str();

    system(str_latest_command.c_str());
}