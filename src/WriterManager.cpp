#include "WriterManager.hpp"
#include <iostream>

using namespace std;

WriterManager::WriterManager(uint64_t n_images, std::string dataset_name):
    n_images(n_images), running_flag(true), n_received_frames(0), n_written_frames(0)
{
    #ifdef DEBUG_OUTPUT
        cout << "[WriterManager::WriterManager] Writer manager for n_images " << n_images << endl;
        cout << "[WriterManager::WriterManager] Using dataset_name " << dataset_name << endl;
    #endif

    // TODO: Remove this. This is needed only for testing.
    parameters.insert({"sl2wv", 1.0});
    parameters.insert({"sl0ch", 1.0});
    parameters.insert({"sl2wh", 1.0});
    parameters.insert({"temp_mono_cryst_1", 1.0});
    parameters.insert({"harmonic", 2});
    parameters.insert({"mokev", 1.0});
    parameters.insert({"sl2cv", 1.0});
    parameters.insert({"bpm4_gain_setting", 1.0});
    parameters.insert({"mirror_coating", string("test_char")});
    parameters.insert({"samx", 1.0});
    parameters.insert({"sample_name", string("test_char")});
    parameters.insert({"bpm5y", 1.0});
    parameters.insert({"sl2ch", 1.0});
    parameters.insert({"curr", 1.0});
    parameters.insert({"bs2_status", string("test_char")});
    parameters.insert({"bs2y", 1.0});
    parameters.insert({"diode", 1.0});
    parameters.insert({"samy", 1.0});
    parameters.insert({"sl4ch", 1.0});
    parameters.insert({"sl4wh", 1.0});
    parameters.insert({"temp_mono_cryst_2", 1.0});
    parameters.insert({"sl3wh", 1.0});
    parameters.insert({"mith", 1.0});
    parameters.insert({"bs1_status", string("test_char")});
    parameters.insert({"bpm4s", 1.0});
    parameters.insert({"sl0wh", 1.0});
    parameters.insert({"bpm6z", 1.0});
    parameters.insert({"bs1y", 1.0});
    parameters.insert({"scan", string("test_char")});
    parameters.insert({"bpm5_gain_setting", 1.0});
    parameters.insert({"bpm4z", 1.0});
    parameters.insert({"bpm4x", 1.0});
    parameters.insert({"date", string("test_char")});
    parameters.insert({"mibd", 1.0});
    parameters.insert({"temp", 1.0});
    parameters.insert({"idgap", 1.0});
    parameters.insert({"sl4cv", 1.0});
    parameters.insert({"sl1wv", 1.0});
    parameters.insert({"sl3wv", 1.0});
    parameters.insert({"sl1ch", 1.0});
    parameters.insert({"bs2x", 1.0});
    parameters.insert({"bpm6_gain_setting", 1.0});
    parameters.insert({"bpm4y", 1.0});
    parameters.insert({"bpm6s", 1.0});
    parameters.insert({"sample_description", string("test_char")});
    parameters.insert({"bpm5z", 1.0});
    parameters.insert({"moth1", 1.0});
    parameters.insert({"sec", 1.0});
    parameters.insert({"sl3cv", 1.0});
    parameters.insert({"bs1x", 1.0});
    parameters.insert({"bpm6_saturation_value", 1.0});
    parameters.insert({"bpm5s", 1.0});
    parameters.insert({"mobd", 1.0});
    parameters.insert({"sl1wh", 1.0});
    parameters.insert({"sl4wv", 1.0});
    parameters.insert({"bs2_det_dist", 1.0});
    parameters.insert({"bpm5_saturation_value", 1.0});
    parameters.insert({"fil_comb_description", string("test_char")});
    parameters.insert({"bpm5x", 1.0});
    parameters.insert({"bpm4_saturation_value", 1.0});
    parameters.insert({"bs1_det_dist", 1.0});
    parameters.insert({"sl3ch", 1.0});
    parameters.insert({"bpm6y", 1.0});
    parameters.insert({"sl1cv", 1.0});
    parameters.insert({"bpm6x", 1.0});
    parameters.insert({"ftrans", 1.0});
    parameters.insert({"samz", 1.0});
}

void WriterManager::stop()
{
    #ifdef DEBUG_OUTPUT
        cout << "[WriterManager::stop] Stopping the writer manager." << endl;
    #endif

    running_flag = false;
}

string WriterManager::get_status()
{
    if (running_flag) {
        return "receiving";
    } else if (n_received_frames > n_written_frames) {
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

std::map<std::string, boost::any>& WriterManager::get_parameters(){
    return parameters;
}

void WriterManager::set_parameters(map<string, boost::any>& new_parameters)
{
    for (auto parameter : new_parameters) {
        string parameter_name = parameter.first;
        auto parameter_value = parameter.second;

        parameters[parameter_name] = parameter_value;
    }
}

bool WriterManager::is_running()
{
    // Take into account n_images only if it is <> 0.
    if (n_images && n_received_frames.load() >= n_images) {
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