#include "BufferUtils.hpp"
#include <sstream>
#include <fstream>
#include <config.hpp>

using namespace std;

string BufferUtils::get_filename(
        std::string root_folder,
        std::string device_name,
        uint64_t pulse_id)
{
    uint64_t folder_base = pulse_id / core_buffer::FOLDER_MOD;
    folder_base *= core_buffer::FOLDER_MOD;

    uint64_t file_base = pulse_id / core_buffer::FILE_MOD;
    file_base *= core_buffer::FILE_MOD;

    stringstream folder;
    folder << root_folder << "/";
    folder << device_name << "/";
    folder << folder_base << "/";
    folder << file_base << core_buffer::FILE_EXTENSION;

    return folder.str();
}

size_t BufferUtils::get_file_frame_index(uint64_t pulse_id)
{
    uint64_t file_base = pulse_id / core_buffer::FILE_MOD;
    file_base *= core_buffer::FILE_MOD;

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

string BufferUtils::get_latest_file(const string& latest_filename)
{
    std::ifstream latest_input_file;
    latest_input_file.open(latest_filename);

    std::stringstream strStream;
    strStream << latest_input_file.rdbuf();
    std::string filename = strStream.str();

    return filename.substr(0, filename.size()-1);
}

vector<BufferUtils::path_sufix> BufferUtils::get_path_suffixes (
        const uint64_t start_pulse_id,
        const uint64_t stop_pulse_id)
{
    vector<BufferUtils::path_sufix> result;

    uint64_t start_file_base = start_pulse_id / core_buffer::FILE_MOD;
    start_file_base *= core_buffer::FILE_MOD;

    for (
            uint64_t first_pulse_id=start_file_base;
            first_pulse_id <= stop_pulse_id;
            first_pulse_id += core_buffer::FILE_MOD) {

        uint64_t folder_base = first_pulse_id / core_buffer::FOLDER_MOD;
        folder_base *= core_buffer::FOLDER_MOD;

        uint64_t file_base = first_pulse_id / core_buffer::FILE_MOD;
        file_base *= core_buffer::FILE_MOD;

        stringstream folder;
        folder << folder_base << "/";
        folder << file_base << core_buffer::FILE_EXTENSION;

        result.emplace_back<BufferUtils::path_sufix>(
                {first_pulse_id,
                 first_pulse_id+core_buffer::FILE_MOD-1,
                 folder.str()});
    }

    return result;
}