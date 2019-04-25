
#ifndef PSIWRITER_H
#define PSIWRITER_H

#include <chrono>
#include "date.h"
#include <boost/thread.hpp>

#include "WriterManager.hpp"
#include "H5Format.hpp"
#include "RingBuffer.hpp"
#include "MetadataBuffer.hpp"


class PSIWriter 
{
    RingBuffer& ring_buffer;
    const H5Format& format;
    hsize_t frames_per_file;

    protected:
        boost::thread writing_thread;

        void write_h5(WriterManager& writer_manager,
                      std::string output_file, 
                      uint64_t n_frames);
        void write_h5_format(H5::H5File& file);

    public:
        PSIWriter(RingBuffer& ring_buffer, 
                  const H5Format& format, 
                  hsize_t frames_per_file=0);

        void run_writer(WriterManager& writer_manager, 
                        std::string output_file, 
                        uint64_t n_frames);

        void join_writer();
};

#endif
