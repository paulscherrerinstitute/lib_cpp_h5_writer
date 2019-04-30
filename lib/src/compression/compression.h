#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <cstddef>
#include <cstdint>

namespace compression {

    size_t get_lz4_max_buffer_size(size_t n_elements, size_t element_size);
    
    size_t compress_lz4(const char* data,
                        size_t n_elements, 
                        size_t element_size,
                        char* buffer,
                        size_t buffer_size);

    size_t decompress_lz4(const char* compressed_data,
                          size_t compressed_size, 
                          char* data);

    size_t get_bitshuffle_max_buffer_size(size_t n_elements, 
                                          size_t element_size);

    size_t compress_bitshuffle(const char* data, 
                               size_t n_elements, 
                               size_t element_size, 
                               char* buffer);

    size_t decompress_bitshuffle(const char* compressed_data, 
                                 size_t compressed_size,
                                 size_t n_elements, 
                                 size_t element_size, 
                                 char* data);
};

#endif
