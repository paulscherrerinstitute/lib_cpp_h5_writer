#ifndef METADATABUFFER_H
#define METADATABUFFER_H

#include <unordered_map>
#include <string>

#include "ZmqReceiver.hpp"

class MetadataBuffer
{
	const uint64_t n_images;
	const std::shared_ptr<std::unordered_map<std::string, HeaderDataType>> header_values_type;

	protected:
		std::unordered_map<std::string, std::shared_ptr<char>> metadata_buffer;
		std::unordered_map<std::string, size_t> metadata_length_bytes;

	public:
		MetadataBuffer(uint64_t n_images, std::shared_ptr<std::unordered_map<std::string, HeaderDataType>> header_values_type);
		void add_metadata_to_buffer(std::string name, uint64_t frame_index, const char* data);
		std::shared_ptr<char> get_metadata_values(std::string name);
		std::shared_ptr<std::unordered_map<std::string, HeaderDataType>> get_header_values_type();
		uint64_t get_n_images();
};

#endif
