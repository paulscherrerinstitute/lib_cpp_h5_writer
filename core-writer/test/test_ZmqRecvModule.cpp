#include "gtest/gtest.h"
#include "ZmqRecvModule.hpp"

#include "RingBuffer.hpp"

using namespace std;

TEST(ZmqRecvModule, basic_interaction)
{
    uint8_t n_receivers = 4;

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

    //ZmqRecvModule zmq_recv_module(ring_buffer,header_values,);


}
