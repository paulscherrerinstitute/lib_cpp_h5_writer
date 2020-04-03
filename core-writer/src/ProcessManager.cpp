#include <cstdlib>
#include <chrono>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <boost/thread.hpp>
#include <future>
#include <vector>

#include "RestApi.hpp"
#include "ProcessManager.hpp"
#include "config.hpp"
#include "BufferedWriter.hpp"
#include "compression.hpp"

using namespace std;

ProcessManager::ProcessManager(
    WriterManager& writer_manager,
    ZmqReceiver& receiver,
    RingBuffer& ring_buffer, 
    const H5Format& format,
    uint16_t rest_port,
    const string& bsread_rest_address,
    hsize_t frames_per_file) :
        writer_manager(writer_manager),
        receiver(receiver),
        ring_buffer(ring_buffer),
        format(format),
        rest_port(rest_port), 
        bsread_rest_address(bsread_rest_address),
        frames_per_file(frames_per_file)
{
}


void ProcessManager::run_receivers(uint8_t n_receiving_threads)
{


    RestApi::start_rest_api(writer_manager, rest_port);


    // In case SIGINT stopped the rest_api.
    writer_manager.stop();


    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ProcessManager::run_writer] Writer properly stopped." << endl;
    #endif
}
