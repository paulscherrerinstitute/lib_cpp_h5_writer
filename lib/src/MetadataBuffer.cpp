#include <iostream>
#include <stdexcept>

#include "date.h"
#include "MetadataBuffer.hpp"

using namespace std;

MetadataBuffer::MetadataBuffer(uint64_t n_images, shared_ptr<unordered_map<string, HeaderDataType>> header_values_type) :
    n_images(n_images), header_values_type(header_values_type)
{
    if (header_values_type) {
        for (const auto& header_type : *header_values_type) {
            auto& name = header_type.first;
            auto& header_data_type = header_type.second;

            size_t bytes_size_per_frame = header_data_type.value_shape * header_data_type.value_bytes_size;
            size_t buffer_size_bytes = n_images * bytes_size_per_frame;

            shared_ptr<char> buffer(new char[buffer_size_bytes](), std::default_delete<char[]>());
            metadata_buffer.insert({name, buffer});
            metadata_length_bytes.insert({name, bytes_size_per_frame});
        }
    }
}

void MetadataBuffer::add_metadata_to_buffer(string name, uint64_t frame_index, const char* data, uint64_t initial_frame_offset)
{
    if (frame_index >= n_images + initial_frame_offset) {
        stringstream error_message;
        using namespace date;
        error_message << "[" << std::chrono::system_clock::now() << "] ";
        error_message << "Requested frame_index " << frame_index << " ";
        error_message << "for metadata " << name << " in metadata buffer is out of range. ";
        error_message << "Number of available slots: " << n_images << endl;

        throw runtime_error(error_message.str());
    }

    auto metadata = metadata_buffer.find(name);

    if (metadata == metadata_buffer.end()) {
        stringstream error_message;
        using namespace date;
        error_message << "[" << std::chrono::system_clock::now() << "] ";
        error_message << "Cannot add data to metadata buffer because '" << name << "' is not declared."<< endl;

        throw runtime_error(error_message.str());
    }

    size_t bytes_size_per_frame = metadata_length_bytes.at(name);
    size_t buffer_offset = (frame_index-initial_frame_offset) * bytes_size_per_frame;

    char* buffer = metadata->second.get();
    buffer += buffer_offset;

    memcpy(buffer, data, bytes_size_per_frame); 
}

shared_ptr<char> MetadataBuffer::get_metadata_values(string name)
{
    auto metadata = metadata_buffer.find(name);

    if (metadata == metadata_buffer.end()) {
       stringstream error_message;
       using namespace date;
       error_message << "[" << std::chrono::system_clock::now() << "] ";
       error_message << "Cannot get data from metadata buffer because '" << name << "' is not declared."<< endl;

       throw runtime_error(error_message.str());
    }

    return metadata->second;
}

shared_ptr<unordered_map<string, HeaderDataType>> MetadataBuffer::get_header_values_type()
{
    return header_values_type;
}

uint64_t MetadataBuffer::get_n_images()
{
    return n_images;
}

