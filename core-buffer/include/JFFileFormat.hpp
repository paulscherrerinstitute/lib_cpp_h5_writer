#ifndef JFFILEFORMAT_HPP
#define JFFILEFORMAT_HPP

#include "jungfrau.hpp"

#pragma pack(push)
#pragma pack(1)
struct JFFileFormat {
    uint64_t pulse_id;
    uint64_t frame_id;
    uint32_t daq_rec;
    uint16 n_recv_packets;
    char* data[JUNGFRAU_DATA_BYTES_PER_FRAME];
};
#pragma pack(pop)

#endif // JFFILEFORMAT_HPP