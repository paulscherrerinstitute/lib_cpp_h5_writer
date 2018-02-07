#include <sstream>

#include "WriterManager.hpp"
#include "H5Format.hpp"
#include "RingBuffer.hpp"

namespace ProcessManager 
{
    void run_writer(WriterManager& manager, const H5Format& format, const std::string& connect_address, uint16_t rest_port);
    void receive_zmq(WriterManager& manager, RingBuffer& ring_buffer, const std::string& connect_address, 
        int n_io_threads=1, int receive_timeout=-1);
    void write_h5(WriterManager& manager, const H5Format& format, RingBuffer& ring_buffer);
};