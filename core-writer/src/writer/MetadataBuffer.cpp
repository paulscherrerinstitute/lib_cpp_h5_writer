#include <iostream>
#include <stdexcept>

#include "date.h"
#include "MetadataBuffer.hpp"

using namespace std;

MetadataBuffer::MetadataBuffer(
        const uint64_t n_slots,
        const header_map& header_values_type) :
            n_slots_(n_slots),
            header_values_type_(header_values_type)
{
    for (const auto& header_type : header_values_type_) {
        auto& name = header_type.first;
        auto& header_data_type = header_type.second;

        size_t bytes_size_per_frame =
                header_data_type.value_shape * header_data_type.value_bytes_size;
        size_t buffer_size_bytes = n_slots_ * bytes_size_per_frame;

        shared_ptr<char> buffer(
                new char[buffer_size_bytes](), std::default_delete<char[]>());

        metadata_buffer.insert({name, buffer});
        metadata_length_bytes.insert({name, bytes_size_per_frame});
    }
}

void MetadataBuffer::add_metadata_to_buffer(
        const string& name,
        const uint64_t frame_index,
        const char* data)
{
    if (frame_index >= n_slots_) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono;
        err_msg << "[" << system_clock::now() << "] ";
        err_msg << "[MetadataBuffer::add_metadata_to_buffer]";
        err_msg << " Requested frame_index " << frame_index ;
        err_msg << " is out of range.";
        err_msg << " Available n_slots_ " << n_slots_ << endl;

        throw runtime_error(err_msg.str());
    }

    auto metadata = metadata_buffer.find(name);

    if (metadata == metadata_buffer.end()) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono;
        err_msg << "[" << system_clock::now() << "] ";
        err_msg << "[MetadataBuffer::add_metadata_to_buffer]";
        err_msg << " Undeclared header metadata " << name << endl;

        throw runtime_error(err_msg.str());
    }

    size_t bytes_size_per_frame = metadata_length_bytes.at(name);
    size_t buffer_offset = frame_index * bytes_size_per_frame;

    char* buffer = metadata->second.get();
    buffer += buffer_offset;

    memcpy(buffer, data, bytes_size_per_frame);
}

shared_ptr<char> MetadataBuffer::get_metadata_values(string name)
{
    auto metadata = metadata_buffer.find(name);

    if (metadata == metadata_buffer.end()) {
       stringstream err_msg;

       using namespace date;
       using namespace chrono;
       err_msg << "[" << system_clock::now() << "] ";
       err_msg << "[MetadataBuffer::get_metadata_values]";
       err_msg << " Undeclared header metadata " << name << endl;

       throw runtime_error(err_msg.str());
    }

    return metadata->second;
}

const MetadataBuffer::header_map& MetadataBuffer::get_header_values_type()
{
    return header_values_type_;
}

uint64_t MetadataBuffer::get_n_slots()
{
    return n_slots_;
}