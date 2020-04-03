#ifndef ZMQRECEIVER_H
#define ZMQRECEIVER_H

#include <string>
#include <memory>
#include <tuple>
#include <zmq.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include <boost/property_tree/json_parser.hpp>
#include <chrono>
#include "date.h"

#include "RingBuffer.hpp"
#include "config.hpp"

struct HeaderDataType
{
    std::string type;
    size_t value_shape;
    std::string endianness;
    size_t value_bytes_size;
    bool is_array;

    HeaderDataType(const std::string& type);
    HeaderDataType(const std::string& type, size_t shape);
};

size_t get_type_byte_size(const std::string& type);

void copy_value_to_buffer(
        const char* buffer, size_t offset,
        const boost::property_tree::ptree& json_value,
        const HeaderDataType& header_data_type);

std::shared_ptr<char> get_value_from_json(
        const boost::property_tree::ptree& json_header,
        const std::string& name,
        const HeaderDataType& header_data_type);

class ZmqReceiver
{
    typedef std::unordered_map<std::string, HeaderDataType> header_map;

    const header_map& header_values_type_;
    zmq::context_t context_;
    zmq::socket_t socket_;
    zmq::message_t message_header_;
    zmq::message_t message_data_;

    boost::property_tree::ptree json_header;

    public:
        ZmqReceiver(
                const header_map& header_values_type,
                const int n_io_threads=config::zmq_n_io_threads
                );

        void connect(
                const std::string& connect_address,
                const int receive_timeout=config::zmq_receive_timeout);

        void disconnect();

        std::shared_ptr<FrameMetadata> read_json_header(const std::string& header);

        std::pair<std::shared_ptr<FrameMetadata>, char*> receive();
};

#endif
