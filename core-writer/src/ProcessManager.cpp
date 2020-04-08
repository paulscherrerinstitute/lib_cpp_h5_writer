#include <iostream>
#include "RestApi.hpp"
#include "ProcessManager.hpp"


using namespace std;

ProcessManager::ProcessManager(
        H5WriteModule& write_module,
        ZmqRecvModule& recv_module) :
            write_module_(write_module),
            recv_module_(recv_module)
{
}

void ProcessManager::start_rest_api(const uint16_t rest_port)
{
    RestApi::start_rest_api(*this, rest_port);

    // In case SIGINT stopped the rest_api.
    stop_receiving();
    stop_writing();

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ProcessManager::start_rest_api]";
        cout << " Server stopped." << endl;
    #endif
}

void ProcessManager::start_writing(
        const string& output_file,
        const int n_frames,
        const int user_id)
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ProcessManager::start_writing]";
        cout << " output_file " << output_file;
        cout << " n_frames " << n_frames;
        cout << " user_id " << user_id << endl;
    #endif

    if (!recv_module_.is_receiving()) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono;
        err_msg << "[" << system_clock::now() << "] ";
        err_msg << "[ProcessManager::start_writing]";
        err_msg << " Cannot start writing. ";
        err_msg << " Must start receiving first." << endl;

        throw runtime_error(err_msg.str());
    }

    recv_module_.stop_saving_and_clear_buffer();

    write_module_.start_writing(output_file, n_frames, user_id);
    recv_module_.start_saving();
}

void ProcessManager::stop_writing()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ProcessManager::stop_writing]" << endl;
    #endif

    write_module_.stop_writing();
    recv_module_.stop_saving_and_clear_buffer();
}

void ProcessManager::start_receiving(
        const string& connect_address,
        const int n_receiving_threads)
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ProcessManager::start_receiving]" << endl;
        cout << " connect_address " << connect_address;
        cout << " n_receiving_threads " << n_receiving_threads;
    #endif

    recv_module_.start_recv(
            connect_address,
            static_cast<uint8_t>(n_receiving_threads));
}

void ProcessManager::stop_receiving()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ProcessManager::stop_receiving]" << endl;
    #endif

    recv_module_.stop_recv();
}

string ProcessManager::get_status()
{
    if (write_module_.is_writing()) {
        return "writing";
    }

    if (recv_module_.is_receiving()) {
        return "ready";
    }

    return "idle";
}

unordered_map<std::string, float> ProcessManager::get_statistics()
{
    // TODO: Implement statistics.
    return {};
}