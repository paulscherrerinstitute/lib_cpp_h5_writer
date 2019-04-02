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

void copy_value_to_buffer(const char* buffer, size_t offset, const boost::property_tree::ptree& json_value, 
    const HeaderDataType& header_data_type);

std::shared_ptr<char> get_value_from_json(const boost::property_tree::ptree& json_header, 
    const std::string& name, const HeaderDataType& header_data_type);

class ZmqReceiver
{
    const std::string connect_address;
    const int n_io_threads;
    const int receive_timeout;
    std::shared_ptr<zmq::socket_t> receiver = NULL;
    std::shared_ptr<zmq::context_t> context = NULL;
    zmq::message_t message_header;
    zmq::message_t message_data;
    boost::property_tree::ptree json_header;

    std::shared_ptr<std::unordered_map<std::string, HeaderDataType>> header_values_type = NULL;

    public:
        ZmqReceiver(const std::string& connect_address, const int n_io_threads, const int receive_timeout,
            std::shared_ptr<std::unordered_map<std::string, HeaderDataType>> header_values_type=NULL);

        ZmqReceiver(const ZmqReceiver& other);

        virtual ~ZmqReceiver(){};

        void connect();

        std::shared_ptr<FrameMetadata> read_json_header(const std::string& header);

        std::pair<std::shared_ptr<FrameMetadata>, char*> receive();

        const std::shared_ptr<std::unordered_map<std::string, HeaderDataType>> get_header_values_type() const;

};

#endif
