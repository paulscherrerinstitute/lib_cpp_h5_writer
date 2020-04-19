#ifndef FASTH5WRITER_HPP
#define FASTH5WRITER_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <H5Cpp.h>

class FastH5Writer {

    const uint16_t CHUNKING_FACTOR = 1;

    const size_t n_frames_per_file_;
    const uint16_t y_frame_size_;
    const uint16_t x_frame_size_;
    const size_t frame_bytes_size_;

    std::string current_output_filename_;
    H5::H5File current_output_file_;
    H5::DataSet current_image_dataset_;
    uint64_t current_pulse_id_;
    size_t current_frame_index_;

    void create_image_dataset();

public:
    FastH5Writer(
            const size_t n_frames_per_file,
            const uint16_t y_frame_size,
            const uint16_t x_frame_size);

    template <class T> void add_metadata(const std::string& metadata_name);

    void set_pulse_id(const uint64_t pulse_id);

    void write_data(const char* buffer);
    template <class T> void write_metadata(
            const std::string& name, const T& value);

};


#endif //FASTH5WRITER_HPP
