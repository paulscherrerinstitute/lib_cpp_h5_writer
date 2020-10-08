#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include "WriterManager.hpp"
#include "H5Format.hpp"
#include "RingBuffer.hpp"
#include "ZmqReceiver.hpp"
#include <chrono>
#include "date.h"
#include <boost/property_tree/json_parser.hpp>

#include <boost/property_tree/json_parser.hpp>

class ProcessManager 
{
    WriterManager& writer_manager;
    ZmqReceiver& receiver;
    RingBuffer& ring_buffer;
    const H5Format& format;

    uint16_t rest_port;
    const std::string& bsread_rest_address;
    hsize_t frames_per_file;
    uint16_t adjust_n_frames;

    void notify_first_pulse_id(uint64_t pulse_id);
    void notify_last_pulse_id(uint64_t pulse_id);
    void notify_pco_client_end(uint64_t written_frames,
                               uint64_t lost_frames,
                               std::string end_time,
                               std::string start_time,
                               float duration,
                               size_t n_frames,
                               size_t n_frames_offset,
                               std::string output_file,
                               int user_id,
                               std::string dataset_name);
    void notify_pco_client_error(std::string error_msg);

    public:
        ProcessManager(WriterManager& writer_manager, ZmqReceiver& receiver,
            RingBuffer& ring_buffer, const H5Format& format, uint16_t rest_port, 
            const std::string& bsread_rest_address, hsize_t frames_per_file=0, uint16_t adjust_n_frames=0
            );

        void run_writer();

        void receive_zmq();

        void write_h5();

        void write_h5_format(H5::H5File& file);
};

#endif