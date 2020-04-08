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
    // writer_manager.stop();


    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ProcessManager::start]";
        cout << " Server stopped." << endl;
    #endif
}

void ProcessManager::start_writing(
        const string& output_file,
        const int n_frames,
        const int user_id)
{
    recv_module_.start_saving();
    write_module_.start_writing(output_file, n_frames, user_id);
}

void ProcessManager::stop_writing()
{
    recv_module_.stop_saving();
    write_module_.stop_writing();
}

void ProcessManager::start_receiving(
        const string& connect_address,
        const int n_receiving_threads)
{
    recv_module_.start_recv(
            connect_address,
            static_cast<uint8_t>(n_receiving_threads));
}

void ProcessManager::stop_receiving()
{
    recv_module_.stop_recv();
}