#ifndef FASTH5WRITER_HPP
#define FASTH5WRITER_HPP

#include <vector>

template <class B>
class FastH5Writer {
public:
    FastH5Writer(
            const uint16_t n_frames_per_file,
            const std::vector<uint16_t>& frame_size
            );

    template <class T> void add_metadata(const std::string& metadata_name);

    void set_pulse_id(const uint64_t pulse_id);

    void write_data(const char* buffer);
    template <class T> void write_metadata(
            const std::string& name, const T& value);

};


#endif //FASTH5WRITER_HPP
