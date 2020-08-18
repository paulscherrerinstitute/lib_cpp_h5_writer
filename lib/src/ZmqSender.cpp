#include <iostream>
#include <stdexcept>
#include <string>
#include <boost/algorithm/string.hpp>


#include "ZmqSender.hpp"
#include "config.hpp"
#include "H5Format.hpp"

using namespace std;
namespace pt = boost::property_tree;

ZmqSender::ZmqSender(const std::string& connect_address, const int n_io_threads) :
        connect_address(connect_address), n_io_threads(n_io_threads), sender(NULL)
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[ZmqSender::ZmqSender] Creating ZMQ sender with";
        cout << " connect_address " << connect_address;
        cout << " n_io_threads " << n_io_threads;
        cout << " receive_timeout " << config::zmq_receive_timeout;
        cout << endl;
    #endif
}

void ZmqSender::bind()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        std::cout <<  "[" << std::chrono::system_clock::now() << "]";
        std::cout <<  "[ZmqSender::bind] Binding to address " << connect_address;
        std::cout <<  " with n_io_threads " << n_io_threads << endl;
    #endif

    context = make_shared<zmq::context_t>(n_io_threads);
    sender = make_shared<zmq::socket_t>(*context, ZMQ_PUB);

    sender->bind(connect_address);
    //  Ensure subscriber connection has time to complete
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

bool ZmqSender::get_stat() const
{
    return stat;
}

std::string ZmqSender::get_mode() const
{
    return mode;
}

bool ZmqSender::get_valid_tcp_stats_address() const{
    std::string t("tcp://");
    bool b = boost::algorithm::contains(connect_address, t);
    return b;
}

void ZmqSender::set_stat_mode(const bool stat_param, const std::string& mode_indication){
    stat = stat_param;
    mode = mode_indication;
}


void ZmqSender::send(const std::string& filter, const std::string& message_data)
{
    if (!sender) {
        stringstream error_message;
        using namespace date;
        error_message << "[" << std::chrono::system_clock::now() << "]";
        error_message << "[ZmqSender::send] Cannot send before connecting. ";
        error_message << "Connect first." << endl;

        throw runtime_error(error_message.str());
    }
    
    // Send the message 
    // rv is the return value
    zmq::message_t message_filter(filter.size());
    memcpy (message_filter.data(), filter.data(), filter.size());
    zmq::message_t message_content(message_data.size());
    memcpy (message_content.data(), message_data.data(), message_data.size());
    
    auto rv0 = sender->send(message_filter, ZMQ_SNDMORE | ZMQ_NOBLOCK);
    auto rv1 = sender->send(message_content, ZMQ_NOBLOCK);
    // verifies the return value
    if (!rv0 || !rv1) {
        using namespace date;
        std::cout << "[" << std::chrono::system_clock::now() << "]";
        std::cout << "[ZmqSender::send] Error while sending statistics via ZMQ. " << endl; 
    }
}