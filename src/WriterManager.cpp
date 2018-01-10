#include "WriterManager.hpp"
#include <iostream>

using namespace std;

WriterManager::WriterManager(uint64_t n_images):
    n_images(n_images), running_flag(true), n_received_frames(0), n_written_frames(0)
{
    #ifdef DEBUG_OUTPUT
        cout << "[WriterManager::WriterManager] Writer manager for n_images " << n_images << endl;
    #endif
}

void WriterManager::stop()
{
    running_flag = false;
}

string WriterManager::get_status()
{
    if (running_flag) {
        return "receiving";
    } else if (n_received_frames == n_written_frames) {
        return "writing";
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

map<string, string>& WriterManager::get_parameters()
{
    // TODO: Implement this.
    map<string, string> result = {};
    return result;
}

void WriterManager::set_parameters(map<string, string>& new_parameters)
{
    // TODO: Implement this.
}

bool WriterManager::is_running()
{
    // Take into account n_images only if it is <> 0.
    if (n_images && n_received_frames.load() > n_images) {
        running_flag = false;
    }

    return running_flag.load();
}

void WriterManager::received_frame(size_t frame_index)
{
    n_received_frames++;
}

void WriterManager::written_frame(size_t frame_index)
{
    n_written_frames++;
}