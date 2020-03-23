#include "compression.h"

#include <stdexcept>
#include <arpa/inet.h>

extern "C"{
    #include "bitshuffle/lz4.h"
    #include "bitshuffle/bitshuffle.h"
}

using namespace std;

#define is_little_endian htonl(1) != 1

size_t compression::get_lz4_max_buffer_size(size_t n_elements,
                                       size_t element_size) 
{
        size_t n_bytes = n_elements * element_size;
        return static_cast<size_t>(LZ4_compressBound(n_bytes)) + 4;
}

size_t compression::compress_lz4(const char* data,
                            size_t n_elements, 
                            size_t element_size,
                            char* buffer, 
                            size_t buffer_size)
{
    size_t data_len = n_elements * element_size;

    // The bytes should be in big endian (network order).
    if(is_little_endian) {
        ((uint32_t*)buffer)[0] = htonl(data_len);
    } else {
        ((uint32_t*)buffer)[0] = data_len;
    }

    size_t compressed_size = LZ4_compress_default(data, &buffer[4], data_len, buffer_size-4);

    if(!compressed_size) {
        throw runtime_error("Error while compressing [LZ4] channel:");
    }
    
    return compressed_size+4;

}

size_t compression::decompress_lz4(const char* compressed_data, size_t compressed_size, char* data) {

    uint32_t expected_data_size;

    if(is_little_endian) {
        expected_data_size = ntohl(((uint32_t*)compressed_data)[0]);
    } else {
        expected_data_size = ((uint32_t*)compressed_data)[0];
    }

    // 4 bytes of our header.
    compressed_size -= 4;

    int decompressed_size = LZ4_decompress_safe(&compressed_data[4], data, compressed_size, expected_data_size);

    if (expected_data_size != decompressed_size) {
        throw runtime_error("Expected and decompressed data len do not match.");
    }

    // If the value is not positive, we throw an exception anyway.
    return (size_t) decompressed_size;
}

size_t compression::get_bitshuffle_max_buffer_size(size_t n_elements,
                                              size_t element_size) 
{
    return bshuf_compress_lz4_bound(n_elements, element_size, 0) + 12;
}

size_t compression::compress_bitshuffle(const char* data, size_t n_elements, size_t element_size, char* buffer){

    size_t block_size = bshuf_default_block_size(element_size);

    uint64_t uncompressed_data_len = (uint64_t) n_elements * element_size;
    // The block size has to be multiplied by the elm_size before inserting it into the binary header.
    // https://github.com/kiyo-masui/bitshuffle/blob/04e58bd553304ec26e222654f1d9b6ff64e97d10/src/bshuf_h5filter.c#L167
    uint32_t header_block_size = (uint32_t) block_size * element_size;

    // The system is little endian, convert values to big endian (network order).
    if (is_little_endian) {
        uint32_t high_bytes = htonl((uint32_t)(uncompressed_data_len >> 32));
        uint32_t low_bytes = htonl((uint32_t)(uncompressed_data_len & 0xFFFFFFFFLL));
        uncompressed_data_len = (((uint64_t)low_bytes) << 32) | high_bytes;

        header_block_size = htonl(header_block_size);
    }

    ((int64_t*)buffer)[0] = uncompressed_data_len;
    ((int32_t*)buffer)[2] = header_block_size;

    auto compressed_size = bshuf_compress_lz4(data, &buffer[12], n_elements, element_size, block_size);

    if (compressed_size <= 0) {
        throw runtime_error("Error while compressing [LZ4] channel:");
    }
    
    // If the value is not positive, we throw an exception anyway.
    return (size_t)compressed_size+12;
}

size_t compression::decompress_bitshuffle(const char* compressed_data, size_t compressed_size,
                                     size_t n_elements, size_t element_size, char* data) {

    uint64_t header_expected_data_size = ((uint64_t*)compressed_data)[0];
    uint32_t header_block_size = ((uint32_t*)compressed_data)[2];

    size_t expected_data_size = header_expected_data_size;

    if (is_little_endian) {
        uint32_t high_bytes = ntohl((uint32_t)(header_expected_data_size & 0xFFFFFFFFLL));
        uint32_t low_bytes = ntohl((uint32_t)(header_expected_data_size >> 32));
        expected_data_size = (((uint64_t)high_bytes) << 32) | low_bytes;

        header_block_size = ntohl(header_block_size);
    }

    // The block size has to be multiplied by the elm_size before inserting it into the binary header.
    // https://github.com/kiyo-masui/bitshuffle/blob/04e58bd553304ec26e222654f1d9b6ff64e97d10/src/bshuf_h5filter.c#L167
    size_t block_size = header_block_size / element_size;

    auto n_processed_bytes = bshuf_decompress_lz4(&compressed_data[12], data, n_elements, element_size, block_size);

    // 12 bytes of our header.
    compressed_size -= 12;
    if (compressed_size != n_processed_bytes) {
        throw runtime_error("Compressed and processed data len do not match.");
    }

    return expected_data_size;
}

