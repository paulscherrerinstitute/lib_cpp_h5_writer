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

void H5WriteModule::start_writing()
{
    if (is_writing_ == true) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono;
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[H5WriteModule::start_writing]";
        err_msg << " Writer already running." << endl;

        throw runtime_error(err_msg.str());
    }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[H5WriteModule::start_writing]";
        cout << " Start writing." << endl;
    #endif

   is_writing_ = true;
   writing_thread_ = thread(&H5WriteModule::write_thread, this);
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
