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

	    shared_ptr<char> buffer(new char[buffer_size_bytes], std::default_delete<char[]>());
	    metadata_buffer.insert({name, buffer});
	    metadata_length_bytes.insert({name, bytes_size_per_frame});
	}
    }
}

void MetadataBuffer::add_metadata_to_buffer(string name, uint64_t frame_index, const char* data)
{
    size_t bytes_size_per_frame = metadata_length_bytes.at(name);
    size_t buffer_offset = frame_index * bytes_size_per_frame;

    char* buffer = (metadata_buffer.at(name)).get();
    buffer += buffer_offset;

    memcpy(buffer, data, bytes_size_per_frame); 
}

shared_ptr<char> MetadataBuffer::get_metadata_values(string name)
{
    return metadata_buffer.at(name);
}
