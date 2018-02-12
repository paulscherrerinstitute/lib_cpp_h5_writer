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

#include "RingBuffer.hpp"

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

    std::shared_ptr<std::unordered_map<std::string, std::string>> header_values_type = NULL;

    std::shared_ptr<FrameMetadata> read_json_header(const std::string& header);

    std::shared_ptr<char> get_value_from_json(const boost::property_tree::ptree& json_header, 
        const std::string& name, const std::string& type);

    public:
        ZmqReceiver(const std::string& connect_address, const int n_io_threads, const int receive_timeout);

        virtual ~ZmqReceiver(){};

        void connect();

        std::pair<std::shared_ptr<FrameMetadata>, char*> receive();

        const std::shared_ptr<std::unordered_map<std::string, std::string>> get_header_values_type() const;

};

#endif