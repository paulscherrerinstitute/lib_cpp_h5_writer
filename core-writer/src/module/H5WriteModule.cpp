#include "H5WriteModule.hpp"
#include <iostream>

using namespace std;

H5WriteModule::H5WriteModule(
        RingBuffer& ring_buffer,
        const header_map& header_values,
        const H5Format& format) :
            ring_buffer_(ring_buffer),
            header_values_(header_values),
            format_(format),
            is_writing_(false)
{
}

void H5WriteModule::stop_writing()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[H5WriteModule::stop_writing]";
        cout << " Disable writing." << endl;
    #endif

    is_writing_ = false;

    if (writing_thread_.joinable()) {
        writing_thread_.join();
    }
}
