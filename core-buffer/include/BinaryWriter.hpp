#ifndef BINARYWRITER_HPP
#define BINARYWRITER_HPP

#include <string>
#include "JFFileFormat.hpp"

class BinaryWriter {

    const std::string device_name_;
    const std::string root_folder_;

    std::string current_output_filename_;
    int output_file_fd_;
    std::string latest_filename_;

public:
    BinaryWriter(
            const std::string& device_name,
            const std::string& root_folder);

    void write(const uint64_t pulse_id, const JFFileFormat* buffer);

    void open_file(const std::string& filename);
    void close_current_file();
};


#endif //BINARYWRITER_HPP
