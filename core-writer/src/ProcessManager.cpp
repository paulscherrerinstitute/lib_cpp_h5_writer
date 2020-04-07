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
