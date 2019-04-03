#include <iostream>
#include <sstream>

#include "WriterManager.hpp"

using namespace std;

void writer_utils::set_process_id(int user_id)
{

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[writer_utils::set_process_id] Setting process uid to " << user_id << endl;
    #endif

    if (setgid(user_id)) {
        stringstream error_message;
        using namespace date;
        error_message << "[" << std::chrono::system_clock::now() << "]";
        error_message << "[writer_utils::set_process_id] Cannot set group_id to " << user_id << endl;

        throw runtime_error(error_message.str());
    }

    if (setuid(user_id)) {
        stringstream error_message;
        using namespace date;
        error_message << "[" << std::chrono::system_clock::now() << "]";
        error_message << "[writer_utils::set_process_id] Cannot set user_id to " << user_id << endl;

        throw runtime_error(error_message.str());
    }
}

void writer_utils::create_destination_folder(const string& output_file)
{
    auto file_separator_index = output_file.rfind('/');

    if (file_separator_index != string::npos) {
        string output_folder(output_file.substr(0, file_separator_index));
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[writer_utils::create_destination_folder] Creating folder " << output_folder << endl;

        string create_folder_command("mkdir -p " + output_folder);
        system(create_folder_command.c_str());
    }
}

WriterManager::WriterManager(const unordered_map<string, DATA_TYPE>& parameters_type):
        parameters_type(parameters_type), logs(10), 
        writing_flag(false), running_flag(true), 
        n_frames_to_receive(0), n_frames_to_write(0)
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << 
        cout << "[WriterManager::WriterManager] Writer manager initialized." << endl;
    #endif
}

WriterManager::~WriterManager(){}

void WriterManager::stop()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[WriterManager::stop] Stopping the writer manager." << endl;
    #endif

    running_flag = false;
}

string WriterManager::get_status()
{
    if (writing_flag) {
        return "writing";
    } else if (running_flag) {
        return "ready";
    } else {
        return "Error.. I guess. This shouldn't be possible? Are you sure you are using it correctly?";
    }
}

unordered_map<string, uint64_t> WriterManager::get_statistics() const
{
    unordered_map<string, uint64_t> result = {{"n_received_frames", n_received_frames.load()},
                                    {"n_written_frames", n_written_frames.load()},
                                    {"n_lost_frames", n_lost_frames.load()}};

    return result;
}

unordered_map<string, boost::any> WriterManager::get_parameters()
{
    return parameters;
}

void WriterManager::start(const unordered_map<string, boost::any>& new_parameters)
{

    #ifdef DEBUG_OUTPUT
        stringstream output_message;
        using namespace date;
        output_message << "[" << std::chrono::system_clock::now() << "]";
        output_message << "[WriterManager::sttart] Starting with parameters: ";

        for (const auto& parameter : new_parameters) {
            auto& parameter_name = parameter.first;
            auto& parameter_value = parameter.second;

            output_message << parameter_name << ": " << parameter_value << ", ";
        }

        cout << output_message.str() << endl;
    #endif

    writing_flag = true;
    boost::thread writer_thread(&ProcessManager::write_h5, this, "output_file", 123);
}

const unordered_map<string, DATA_TYPE>& WriterManager::get_parameters_type() const
{
    return parameters_type;
}

bool WriterManager::is_running()
{
    return running_flag.load();
}

bool WriterManager::is_writing() const
{
    return writing_flag.load();
}

bool WriterManager::receive_frame() {
    if (n_frames_to_receive > 0) {
        return (n_frames_to_receive.fetch_sub(1) >= 0);
    }

    return false;
}

bool WriterManager::write_frame() {
    if (n_frames_to_write > 0) {
        return (n_frames_to_write.fetch_sub(1) >= 0);
    }

    return false;
}

void WriterManager::writing_completed() {
    writing_flag = false;    
}
