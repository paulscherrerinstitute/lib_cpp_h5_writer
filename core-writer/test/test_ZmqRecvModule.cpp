#include "gtest/gtest.h"
#include "ZmqRecvModule.hpp"

#include "RingBuffer.hpp"

using namespace std;

TEST(ZmqRecvModule, basic_interaction)
{
    RingBuffer ring_buffer(10);

    size_t n_modules(4);
    unordered_map<string, HeaderDataType> header_values {
            {"pulse_id", HeaderDataType("uint64")},
            {"frame", HeaderDataType("uint64")},
            {"is_good_frame", HeaderDataType("uint64")},
            {"daq_rec", HeaderDataType("int64")},

            {"pulse_id_diff", HeaderDataType("int64", n_modules)},
            {"framenum_diff", HeaderDataType("int64", n_modules)},

            {"missing_packets_1", HeaderDataType("uint64", n_modules)},
            {"missing_packets_2", HeaderDataType("uint64", n_modules)},
            {"daq_recs", HeaderDataType("uint64", n_modules)},

            {"pulse_ids", HeaderDataType("uint64", n_modules)},
            {"framenums", HeaderDataType("uint64", n_modules)},

            {"module_number", HeaderDataType("uint64", n_modules)}
    };

    ZmqRecvModule zmq_recv_module(ring_buffer,header_values);

    uint8_t n_receivers = 4;
    zmq_recv_module.start_recv("tcp://127.0.0.1:10000", n_receivers);

    zmq_recv_module.start_writing();
    zmq_recv_module.stop_writing();

    zmq_recv_module.stop_recv();
}
