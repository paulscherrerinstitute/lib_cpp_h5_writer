#ifndef H5WRITERMODULE_H
#define H5WRITERMODULE_H

#include <thread>
#include "RingBuffer.hpp"
#include "ZmqReceiver.hpp"
#include "H5Format.hpp"

class H5WriteModule {
    typedef std::unordered_map<std::string, HeaderDataType> header_map;

    RingBuffer& ring_buffer_;
    const header_map& header_values_;
    const H5Format& format_;
    std::atomic_bool is_writing_;
    std::thread writing_thread_;

protected:
    void write_thread(
            const std::string& output_file,
            const size_t n_frames,
            const int user_id);

public:
    H5WriteModule(
            RingBuffer& ring_buffer,
            const header_map& header_values,
            const H5Format& format);

    void start_writing(
            const std::string& output_file,
            const size_t n_frames = 0,
            const int user_id = -1
            );

    void stop_writing();

    bool is_writing();
};


#endif //H5WRITERMODULE_H
