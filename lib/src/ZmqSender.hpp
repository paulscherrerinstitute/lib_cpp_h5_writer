#ifndef ZMQSENDER_H
#define ZMQSENDER_H

#include <string>
#include <memory>
#include <tuple>
#include <vector>
#include <memory>
#include <unordered_map>
#include <boost/property_tree/json_parser.hpp>
#include <zmq.hpp>
#include <thread>
#include <chrono>


class ZmqSender
{
    const std::string connect_address;
    const int receive_timeout;
    std::string filter;
    bool stat;
    const int n_io_threads;
    std::string mode;

    std::shared_ptr<zmq::socket_t> sender = NULL;
    std::shared_ptr<zmq::context_t> context = NULL;


    public:
    ZmqSender(const std::string& connect_address, 
                const int n_io_threads, const int receive_timeout);
    virtual ~ZmqSender(){};

    void bind();

    void send(const std::string& filter, const std::string& message_data);

    void set_stat_mode(bool stat, const std::string& mode_indication);

    bool get_stat() const;

    std::string get_mode() const;

};

#endif