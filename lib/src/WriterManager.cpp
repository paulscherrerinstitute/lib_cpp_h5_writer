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
        string create_folder_command("mkdir -p " + output_folder);
        string create_folder_command("mkdir -p " + output_folder);
        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << std::chrono::system_clock::now() << "]";
            cout << "[writer_utils::create_destination_folder] Creating folder " << output_folder << endl;
        #endif
        system(create_folder_command.c_str());
    }
}

WriterManager::WriterManager(const unordered_map<string, DATA_TYPE>& parameters_type, 
    const string& output_file, const string& dataset_name, int user_id, size_t n_frames):
        parameters_type(parameters_type), output_file(output_file), dataset_name(dataset_name), n_frames(n_frames),
        running_flag(true), killed_flag(false), n_received_frames(0), n_written_frames(0), n_lost_frames(0), user_id(user_id)
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[WriterManager::WriterManager] Writer manager for n_frames " << get_n_frames() << endl;
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

void WriterManager::kill()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[WriterManager::kills] Killing writer manager." << endl;
    #endif

    killed_flag = true;

    stop();
}


string WriterManager::get_status()
{
    if (running_flag) {
        return "receiving";
    } else if (n_received_frames.load() > n_written_frames) {
        return "writing";
    } else if (!are_all_parameters_set()) {
        return "waiting for parameters";
    } else {
        return "finished";
    }
}

string WriterManager::get_output_file() const
{
    return output_file;
}

string WriterManager::get_dataset_name() const
{
    return dataset_name;
}


unordered_map<string, uint64_t> WriterManager::get_statistics() const
{
    unordered_map<string, uint64_t> result = {
                                    {"n_written_frames", get_n_written_frames()},
                                    {"n_frames", get_n_frames()},
                                    {"n_received_frames", get_n_received_frames()},
                                    {"user_id", get_user_id()},
                                    {"first_frame_id", n_frames_offset},
                                    {"n_lost_frames", get_n_lost_frames()}};

    return result;
}

unordered_map<string, boost::any> WriterManager::get_parameters()
{
    lock_guard<mutex> lock(parameters_mutex);

    return parameters;
}

void WriterManager::set_processing_rate(float diff)
{
    processing_rate = diff;
}

void WriterManager::set_parameters(const unordered_map<string, boost::any>& new_parameters)
{
    lock_guard<mutex> lock(parameters_mutex);

    #ifdef DEBUG_OUTPUT
        stringstream output_message;
        using namespace date;
        output_message << "[" << std::chrono::system_clock::now() << "]";
        output_message << "[WriterManager::set_parameters] Setting parameters: ";
    #endif

    for (const auto& parameter : new_parameters) {
        auto& parameter_name = parameter.first;
        auto& parameter_value = parameter.second;

        parameters[parameter_name] = parameter_value;

        #ifdef DEBUG_OUTPUT
            output_message << parameter_name << ", ";
        #endif
    }

    #ifdef DEBUG_OUTPUT
        cout << output_message.str() << endl;
    #endif
}

const unordered_map<string, DATA_TYPE>& WriterManager::get_parameters_type() const
{
    return parameters_type;
}

bool WriterManager::is_running()
{
    // Take into account n_frames only if it is <> 0.
    if (n_frames && n_received_frames.load() >= n_frames) {
        running_flag = false;
    }

    return running_flag.load();
}

bool WriterManager::is_killed() const
{
    return killed_flag.load();
}

void WriterManager::received_frame(size_t frame_index)
{
    n_received_frames++;
}

void WriterManager::written_frame(size_t frame_index)
{
    n_written_frames++;
}

void WriterManager::lost_frame(size_t frame_index)
{
    n_lost_frames++;
}

bool WriterManager::are_all_parameters_set()
{
    lock_guard<mutex> lock(parameters_mutex);

    for (const auto& parameter : parameters_type) {
        const auto& parameter_name = parameter.first;

        if (parameters.count(parameter_name) == 0) {
            #ifdef DEBUG_OUTPUT
                using namespace date;
                cout << "[" << std::chrono::system_clock::now() << "]";
                cout << "[WriterManager::are_all_parameters_set] Parameter " << parameter_name << " not set." << endl;
            #endif

            return false;
        }
    }

    return true;
}

size_t WriterManager::get_n_frames() const
{
    return n_frames;
}

void WriterManager::set_n_frames_offset(size_t new_n_frames)
{
    n_frames_offset = new_n_frames;
}

size_t WriterManager::get_n_frames_offset() const
{
    return n_frames_offset;
}
std::string WriterManager::get_filter() const
{
    return "statisticsWriter";
}

void WriterManager::set_stat_flag(bool new_mode, std::string new_cat){
    lock_guard<mutex> lock(statistics_mutex);
    mode_category = std::make_tuple(new_mode, new_cat);
    if (new_cat == "start")
    {
        set_time_start();
    }
    if (new_cat == "end")
    {
        set_time_end();
    }
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[WriterManager::set_stat_flag] Setting flag, statistics should be sent! " << std::get<0>(mode_category) << " " << std::get<1>(mode_category);
        cout << "." << endl;
    #endif
}

std::tuple<bool, std::string> WriterManager::get_stat_flag()
{
    lock_guard<mutex> lock(statistics_mutex);
    return mode_category;
}

int WriterManager::get_user_id() const
{
    return user_id;
}

uint64_t WriterManager::get_n_lost_frames() const
{
    return n_lost_frames;
}

size_t WriterManager::get_n_written_frames() const
{
    return n_written_frames;
}

size_t WriterManager::get_n_received_frames() const
{
    return n_received_frames;
}

size_t WriterManager::get_n_free_slots() const
{
    if (n_frames != 0){
        return n_frames - n_written_frames;
    }else{
        return -1;
    }
}


std::string WriterManager::get_time_end() const
{
    time_t tt;
    tt = std::chrono::system_clock::to_time_t(time_end);
    return ctime(&tt);
}

std::chrono::system_clock::time_point WriterManager::get_time_start_point() const
{
    return time_start;
}

std::string WriterManager::get_time_start() const
{
    time_t tt;
    tt = std::chrono::system_clock::to_time_t(time_start);
    return ctime(&tt);
}

float WriterManager::get_duration() const
{
    auto frame_time_difference = time_end - time_start;
    auto time_diff_ms = std::chrono::duration<float, milli>(frame_time_difference).count() * 0.001;
    return time_diff_ms;
}

void WriterManager::set_time_end()
{
    time_end = std::chrono::system_clock::now();
}

void WriterManager::set_time_start()
{
    time_start = std::chrono::system_clock::now();
}
