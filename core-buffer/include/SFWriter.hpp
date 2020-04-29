#ifndef SFWRITER_HPP
#define SFWRITER_HPP

#include <memory>
#include <string>
#include <H5Cpp.h>
#include "RingBuffer.hpp"

struct DetectorFrame
{
    uint64_t pulse_id;
    uint64_t frame_index;
    uint32_t daq_rec;
    uint16_t n_received_packets;
};

class SFWriter {

    const size_t n_frames_;
    const size_t n_modules_;
    size_t current_write_index_;

    H5::H5File file_;

    H5::DataSet image_dataset_;
    H5::DataSet pulse_id_dataset_;
    H5::DataSet frame_index_dataset_;
    H5::DataSet daq_rec_dataset_;
    H5::DataSet n_received_packets_dataset_;

    std::unique_ptr<char[]> image_buffer_;
    size_t image_buffer_count_;

public:
    SFWriter(
            const std::string& output_file,
            const size_t n_frames,
            const size_t n_modules);
    ~SFWriter();
    void write(const DetectorFrame* metadata, char* data);
    void close_file();
};


#endif //SFWRITER_HPP
