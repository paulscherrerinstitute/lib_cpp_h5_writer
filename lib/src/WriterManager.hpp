#ifndef WRITERMANAGER_H
#define WRITERMANAGER_H

#include <unordered_map>
#include <string>
#include <atomic>
#include <mutex>
#include <boost/any.hpp>
#include <boost/thread.hpp>
#include <chrono>
#include "date.h"
#include <deque>
#include <memory>
#include <list>

#include "ZmqReceiver.hpp"
#include "RingBuffer.hpp"
#include "H5Format.hpp"
#include "compression/compression.h"

namespace writer_utils {
    void set_process_id(int user_id);
    void create_destination_folder(const std::string& output_file);
}

struct WriterManagerLog
{
    std::string filename;
    std::string status;

    uint64_t n_requested_frames;
    uint64_t n_received_frames;
    uint64_t n_written_frames;
};


class WriterManager
{
    // Initialize in constructor.

    std::atomic<bool> running_flag;

    std::atomic<bool> writing_flag;
    std::atomic<bool> receiving_flag;

    std::atomic<int64_t> n_frames_to_receive;
    std::atomic<int64_t> n_frames_to_write;

    protected:
        RingBuffer& ring_buffer;

        ZmqReceiver& receiver;
        uint8_t n_receiving_threads;

        const H5Format& format;
        hsize_t frames_per_file;

        std::list<std::unique_ptr<boost::thread>> receiving_threads;
        boost::thread writing_thread;

        typedef std::unordered_map<std::string, HeaderDataType> header_map;
        std::shared_ptr<header_map> header_values_type = NULL;

        const std::deque<WriterManagerLog> logs;

        void receive_zmq();
        void write_h5(std::string output_file, uint64_t n_frames);

        void write_h5_format(H5::H5File& file);

    public:

        WriterManager(ZmqReceiver& receiver,
                      RingBuffer& ring_buffer, 
                      const H5Format& format, 
                      std::shared_ptr<header_map> header_values_type,
                      uint8_t n_receiving_threads,
                      hsize_t frames_per_file=0);

        virtual ~WriterManager();

        void start(std::string output_file, int n_frames, int user_id);
        void stop();

        std::string get_status();
        std::unordered_map<std::string, uint64_t> get_statistics() const;

        bool receive_frame();
        bool is_running() const;
        bool is_writing() const;

        bool should_write_frame();
        bool should_receive_frame();
        
        void writing_completed();
        void writing_error(std::string error);
};

#endif
