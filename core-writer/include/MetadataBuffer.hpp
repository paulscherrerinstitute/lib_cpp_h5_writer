#ifndef METADATABUFFER_H
#define METADATABUFFER_H

#include <unordered_map>
#include <string>

#include "ZmqReceiver.hpp"

class MetadataBuffer
{
    typedef std::unordered_map<std::string, HeaderDataType> header_map;

	const uint64_t n_slots_;
	const header_map& header_values_type_;

	protected:
		std::unordered_map<std::string, std::shared_ptr<char>> metadata_buffer;
		std::unordered_map<std::string, size_t> metadata_length_bytes;

	public:
		MetadataBuffer(
		        const uint64_t n_slots,
		        const header_map& header_values_type);

		void add_metadata_to_buffer(
		        const std::string& name,
		        const uint64_t frame_index,
		        const char* data);

		std::shared_ptr<char> get_metadata_values(std::string name);
		const header_map& get_header_values_type();
		uint64_t get_n_slots();
};

#endif
