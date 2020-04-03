#include <iostream>
#include <stdexcept>

#include "config.hpp"
#include "ZmqReceiver.hpp"
#include "H5Format.hpp"

using namespace std;
namespace pt = boost::property_tree;

HeaderDataType::HeaderDataType(const std::string& type, size_t shape) : 
        type(type),
        value_shape(shape),
        endianness("little"),
        is_array(true)
{
    value_bytes_size = get_type_byte_size(type);
}

HeaderDataType::HeaderDataType(const std::string& type) : 
        type(type),
        value_shape(1),
        endianness("little"),
        is_array(false)
{
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
        using namespace date;
        error_message << "[" << std::chrono::system_clock::now() << "]";
        error_message << "[ZmqReceiver::get_type_byte_size]";
        error_message << " Unsupported data type " << type << endl;

        throw runtime_error(error_message.str());
    }
}

ZmqReceiver::ZmqReceiver(
        const header_map& header_values_type,
        const int n_io_threads) :
            header_values_type_(header_values_type),
            context_(n_io_threads),
            socket_(context_, ZMQ_PULL),
            message_header_(config::zmq_buffer_size_header),
            message_data_(config::zmq_buffer_size_data)
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ZmqReceiver::ZmqReceiver]";
        cout << " Creating ZMQ receiver with";
        cout << " n_io_threads " << n_io_threads;
        cout << endl;
    #endif
}

void ZmqReceiver::connect(
        const string& connect_address,
        const int receive_timeout)
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ZmqReceiver::connect]";
        cout << " Connecting to address " << connect_address;
        cout << " with receive timeout " << receive_timeout << endl;
    #endif

    socket_.setsockopt(ZMQ_RCVTIMEO, receive_timeout);
    socket_.connect(connect_address);
}

void ZmqReceiver::disconnect()
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ZmqReceiver::disconnect]";
        cout << " Disconnect." << endl;
    #endif

    socket_.close();
    context_.close();
}

pair<shared_ptr<FrameMetadata>, char*> ZmqReceiver::receive()
{
    if (!socket_.connected()) {
        stringstream error_message;
        using namespace date;
        using namespace chrono;
        error_message << "[" << system_clock::now() << "]";
        error_message << "[ZmqReceiver::receive]";
        error_message << " Cannot receive before connecting.";
        error_message << " Connect first." << endl;

        throw runtime_error(error_message.str());
    }

    // Get the message header.
    if (!socket_.recv(&message_header_)){
        return {nullptr, nullptr};
    }

    auto header_string = string(
            static_cast<char*>(message_header.data()),
            message_header.size());
    auto frame_metadata = read_json_header(header_string);

    // Get the message data.
    if (!socket_.recv(&message_data)) {
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ZmqReceiver::receive]";
        cout << " Error while reading from ZMQ.";
        cout << " Frame index " << frame_metadata->frame_index << " lost.";
        cout << " Trying to continue with the next frame." << endl;

        return {nullptr, nullptr};
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


        for (const auto& value_mapping : header_values_type_) {

            const auto& name = value_mapping.first;
            const auto& header_data_type = value_mapping.second;

            auto value = get_value_from_json(
                    json_header, name, header_data_type);

            header_data->header_values.insert(
                {name, value}
            );
        }
        
        return header_data;

    } catch (...) {
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[ZmqReceiver::read_json_header]";
        cout << " Error while interpreting the JSON header. ";
        cout << " Header string: " << header << endl;

        cout << "Expected JSON header format: " << endl;
        for (const auto& value_mapping : header_values_type_) {
            cout << "\t" << value_mapping.first << ":";
            cout << value_mapping.second.type;
            cout << "[" << value_mapping.second.value_shape << "]" << endl;
        }

        throw;
    }
}

void copy_value_to_buffer(
        char* buffer,
        const size_t offset,
        const pt::ptree& json_value,
        const HeaderDataType& header_data_type)
{
    if (header_data_type.type == "uint8") {
        auto value = json_value.get_value<uint8_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value),
                header_data_type.value_bytes_size);

    } else if (header_data_type.type == "uint16") {
        auto value = json_value.get_value<uint16_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value),
                header_data_type.value_bytes_size);

    } else if (header_data_type.type == "uint32") {
        auto value = json_value.get_value<uint32_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value),
                header_data_type.value_bytes_size);

    } else if (header_data_type.type == "uint64") {
        auto value = json_value.get_value<uint64_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value),
                header_data_type.value_bytes_size);

    } else if (header_data_type.type == "int8") {
        auto value = json_value.get_value<int8_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value),
                header_data_type.value_bytes_size);
        
    } else if (header_data_type.type == "int16") {
        auto value = json_value.get_value<int16_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value),
                header_data_type.value_bytes_size);

    } else if (header_data_type.type == "int32") {
        auto value = json_value.get_value<int32_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value),
                header_data_type.value_bytes_size);
    
    } else if (header_data_type.type == "int64") {
        auto value = json_value.get_value<int64_t>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value),
                header_data_type.value_bytes_size);
    
    } else if (header_data_type.type == "float32") {
        auto value = json_value.get_value<float>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value),
                header_data_type.value_bytes_size);

    } else if (header_data_type.type == "float64") {
        auto value = json_value.get_value<double>();
        memcpy(buffer + offset, reinterpret_cast<char*>(&value),
                header_data_type.value_bytes_size);

    } else {
        // We cannot really convert this attribute.
        stringstream error_message;
        using namespace date;
        error_message << "[" << std::chrono::system_clock::now() << "]";
        error_message << "[ZmqReceiver::get_value_from_json]";
        error_message << " Unsupported header data type ";
        error_message << header_data_type.type << endl;

        throw runtime_error(error_message.str());
    }
}

shared_ptr<char> get_value_from_json(
        const pt::ptree& json_header,
        const string& name,
        const HeaderDataType& header_data_type)
{
    char* buffer = new char[
            header_data_type.value_bytes_size * header_data_type.value_shape];

    if (header_data_type.is_array) {
        size_t index = 0;

        for (const auto& item : json_header.get_child(name)) {
            auto offset = index * header_data_type.value_bytes_size;
            copy_value_to_buffer(buffer, offset, item.second, header_data_type);

            ++index;
        }

    } else {
        copy_value_to_buffer(
                buffer, 0, json_header.get_child(name), header_data_type);
    }

    return shared_ptr<char>(buffer, default_delete<char[]>());
}
