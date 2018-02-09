#include <sstream>
#include <memory>
#include <boost/property_tree/json_parser.hpp>

#include "WriterManager.hpp"
#include "H5Format.hpp"
#include "RingBuffer.hpp"

namespace ProcessManager 
{
    void run_writer(WriterManager& manager, const H5Format& format, const std::string& connect_address, uint16_t rest_port);

    void receive_zmq(WriterManager& manager, RingBuffer& ring_buffer, const H5Format& format, 
        const std::string& connect_address, int n_io_threads=1, int receive_timeout=-1);

    void write_h5(WriterManager& manager, const H5Format& format, RingBuffer& ring_buffer);

    std::shared_ptr<FrameMetadata> read_json_header(boost::property_tree::ptree& json_header, 
        const std::string& header, const std::map<std::string, HEADER_DATA_TYPE>& header_data_type);

    boost::any get_value_from_json(const boost::property_tree::ptree& json_header, 
        const string& value_name, const HEADER_DATA_TYPE data_type);
};