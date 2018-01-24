#include <iostream>

#include "WriterManager.hpp"

using namespace std;

WriterManager::WriterManager(map<string, DATA_TYPE>* parameters_type, uint64_t n_images):
    parameters_type(parameters_type), n_images(n_images), running_flag(true), killed_flag(false), n_received_frames(0), n_written_frames(0)
{
    #ifdef DEBUG_OUTPUT
        cout << "[WriterManager::WriterManager] Writer manager for n_images " << n_images << endl;
    #endif
}

void WriterManager::stop()
{
    #ifdef DEBUG_OUTPUT
        cout << "[WriterManager::stop] Stopping the writer manager." << endl;
    #endif

    running_flag = false;
}

void WriterManager::kill()
{
    #ifdef DEBUG_OUTPUT
        cout << "[WriterManager::kills] Killing writer manager." << endl;
    #endif

    killed_flag = true;

    stop();
}

string WriterManager::get_status()
{
    if (running_flag) {
        return "receiving";
    } else if (n_received_frames > n_written_frames) {
        return "writing";
    } else if (!are_all_parameters_set()) {
        return "waiting for parameters";
    } else {
        return "finished";
    }
}

map<string, uint64_t> WriterManager::get_statistics()
{
    map<string, uint64_t> result = {{"n_received_frames", n_received_frames.load()},
                                    {"n_written_frames", n_written_frames.load()},
                                    {"total_expected_frames", n_images}};

    return result;
}

map<string, boost::any> WriterManager::get_parameters(){
    return parameters;
}

void WriterManager::set_parameters(map<string, boost::any>& new_parameters)
{
    lock_guard<mutex> lock(parameters_mutex);

    for (auto parameter : new_parameters) {
        string parameter_name = parameter.first;
        auto parameter_value = parameter.second;

        parameters[parameter_name] = parameter_value;
    }
}

map<string, DATA_TYPE>* WriterManager::get_parameters_type() {
    return parameters_type;
}

bool WriterManager::is_running()
{
    // Take into account n_images only if it is <> 0.
    if (n_images && n_received_frames.load() >= n_images) {
        running_flag = false;
    }

    return running_flag.load();
}

bool WriterManager::is_killed()
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

bool WriterManager::are_all_parameters_set() {
    lock_guard<mutex> lock(parameters_mutex);

    for (auto parameter : *parameters_type) {
        auto parameter_name = parameter.first;

        if (parameters.count(parameter_name) == 0) {
            return false;
        }
    }

    return true;
}