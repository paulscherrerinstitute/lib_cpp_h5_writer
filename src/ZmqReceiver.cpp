#include <iostream>
#include <stdexcept>

#include "config.hpp"
#include "ZmqReceiver.hpp"
#include "H5Format.hpp"

using namespace std;
namespace pt = boost::property_tree;

HeaderDataType::HeaderDataType(const std::string& type, size_t value_shape) : 
    type(type), value_shape(value_shape), endianness("little") {
        value_bytes_size = get_type_byte_size(type);
}

size_t get_type_byte_size(const string& type)
{
    if (type == "uint8" || type== "int8") {
        return 1;

    } else if (type == "uint16" || type == "int16") {
        return 2;

    } else if (type == "uint32" || type == "int32" || type == "float32") {
        return 4;

    } else if (type == "uint64" || type == "int64" || type == "float64") {
        return 8;

    } else {
        stringstream error_message;
        error_message << "[ZmqReceiver::get_type_byte_size] Unsupported data type " << type << endl;

        throw runtime_error(error_message.str());
    }
}

ZmqReceiver::ZmqReceiver(const std::string& connect_address, const int n_io_threads, const int receive_timeout,
    shared_ptr<unordered_map<string, HeaderDataType>> header_values_type) :
        connect_address(connect_address), n_io_threads(n_io_threads), 
        receive_timeout(receive_timeout), receiver(NULL), header_values_type(header_values_type)

{
    #ifdef DEBUG_OUTPUT
        cout << "[ZmqReceiver::ZmqReceiver] Creating ZMQ receiver with";
        cout << " connect_address " << connect_address;
        cout << " n_io_threads " << n_io_threads;
        cout << " receive_timeout " << receive_timeout;
        cout << endl;
    #endif

    message_header = zmq::message_t(config::zmq_buffer_size_header);
    message_data = zmq::message_t(config::zmq_buffer_size_data);
}

void ZmqReceiver::connect()
{
    #ifdef DEBUG_OUTPUT
        cout << "[ZmqReceiver::connect] Connecting to address " << connect_address;
        cout << " with n_io_threads " << n_io_threads << endl;
    #endif

    context = make_shared<zmq::context_t>(n_io_threads);
    receiver = make_shared<zmq::socket_t>(*context, ZMQ_PULL);

    receiver->setsockopt(ZMQ_RCVTIMEO, receive_timeout);
    receiver->connect(connect_address);
}

pair<shared_ptr<FrameMetadata>, char*> ZmqReceiver::receive()
{
    if (!receiver) {
        stringstream error_message;
        error_message << "[ZmqReceiver::receive] Cannot receive before connecting. ";
        error_message << "Connect first." << endl;

        throw runtime_error(error_message.str());
    }

    // Get the message header.
    if (!receiver->recv(&message_header)){
        return {NULL, NULL};
    }

    auto header_string = string(static_cast<char*>(message_header.data()), message_header.size());
    auto frame_metadata = read_json_header(header_string);

    // Get the message data.
    if (!receiver->recv(&message_data)) {
        cout << "[ZmqReceiver::receive] Error while reading from ZMQ. Frame index " << frame_metadata->frame_index << " lost."; 
        cout << " Trying to continue with the next frame." << endl;

        return {NULL, NULL};
    }

    frame_metadata->frame_bytes_size = message_data.size();

    return {frame_metadata, static_cast<char*>(message_data.data())};
}

shared_ptr<FrameMetadata> ZmqReceiver::read_json_header(const string& header)
{   
    try {

        stringstream header_stream;
        header_stream << header << endl;
        pt::read_json(header_stream, json_header);

        auto header_data = make_shared<FrameMetadata>();

        header_data->frame_index = json_header.get<uint64_t>("frame");

        for (const auto& item : json_header.get_child("shape")) {
            header_data->frame_shape.push_back(item.second.get_value<size_t>());
        }

        // Array 1.0 specified little endian as the default encoding.
        header_data->endianness = json_header.get("endianness", "little");

        header_data->type = json_header.get<string>("type");

        if (header_values_type) {
            for (const auto& value_mapping : *header_values_type) {
                
                const auto& name = value_mapping.first;
                const auto& header_data_type = value_mapping.second;

                auto value = get_value_from_json(json_header, name, header_data_type);

                header_data->header_values.insert(
                    {name, value}
                );
            }
        }
        
        return header_data;

    } catch (...) {
        cout << "[ZmqReceiver::read_json_header] Error while interpreting the JSON header. Header string: " << header << endl; 
        cout << "Expected JSON header format: " << endl; 

        if (header_values_type) {
            for (const auto& value_mapping : *header_values_type) {
                cout << "\t" << value_mapping.first << ":" << value_mapping.second.type;
                cout << "[" << value_mapping.second.value_shape << "]" << endl;
            }
        } else {
            cout << "\tExpected header value types is a null pointer." << endl; 
        }
        throw;
    }
}

void copy_value_to_buffer(char* buffer, const size_t offset, const pt::ptree& json_value, const HeaderDataType& header_data_type)
{
    if (header_data_type.type == "uint8") {
        auto value = json_value.get_value<uint8_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value), header_data_type.value_bytes_size);

    } else if (header_data_type.type == "uint16") {
        auto value = json_value.get_value<uint16_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value), header_data_type.value_bytes_size);

    } else if (header_data_type.type == "uint32") {
        auto value = json_value.get_value<uint32_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value), header_data_type.value_bytes_size);

    } else if (header_data_type.type == "uint64") {
        auto value = json_value.get_value<uint64_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value), header_data_type.value_bytes_size);

    } else if (header_data_type.type == "int8") {
        auto value = json_value.get_value<int8_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value), header_data_type.value_bytes_size);
        
    } else if (header_data_type.type == "int16") {
        auto value = json_value.get_value<int16_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value), header_data_type.value_bytes_size);

    } else if (header_data_type.type == "int32") {
        auto value = json_value.get_value<int32_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value), header_data_type.value_bytes_size);
    
    } else if (header_data_type.type == "int64") {
        auto value = json_value.get_value<int64_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value), header_data_type.value_bytes_size);
    
    } else if (header_data_type.type == "float32") {
        auto value = json_value.get_value<float>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value), header_data_type.value_bytes_size);

    } else if (header_data_type.type == "float64") {
        auto value = json_value.get_value<double>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value), header_data_type.value_bytes_size);

    } else {
        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "[ZmqReceiver::get_value_from_json] Unsupported header data type " << header_data_type.type << endl;

        throw runtime_error(error_message.str());
    }
}

shared_ptr<char> ZmqReceiver::get_value_from_json(const pt::ptree& json_header, const string& name, const HeaderDataType& header_data_type) const
{
    char* buffer = new char[header_data_type.value_bytes_size * header_data_type.value_shape];

    if (header_data_type.value_shape == 1) {
        copy_value_to_buffer(buffer, 0, json_header.get_child(name), header_data_type);

    } else {
        size_t index = 0;

        for (const auto& item : json_header.get_child(name)) {
            auto offset = index * header_data_type.value_bytes_size;
            copy_value_to_buffer(buffer, offset, json_header.get_child(name), header_data_type);

            ++index;
        }

    }

    return shared_ptr<char>(buffer, default_delete<char[]>());
}

const shared_ptr<unordered_map<string, HeaderDataType>> ZmqReceiver::get_header_values_type() const
{
    return header_values_type;
}
