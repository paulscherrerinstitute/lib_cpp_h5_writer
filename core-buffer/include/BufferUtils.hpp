#ifndef BUFFER_UTILS_HPP
#define BUFFER_UTILS_HPP

#include <string>
#include <vector>

namespace BufferUtils
{
    const size_t STREAM_BLOCK_SIZE = 200;
    extern const size_t FILE_MOD;
    extern const size_t FOLDER_MOD;
    extern const std::string FILE_EXTENSION;

    #pragma pack(push)
    #pragma pack(1)
    struct FileBufferMetadata {
        // Needed by RingBuffer
        const uint64_t frame_bytes_size = 2*512*1024*STREAM_BLOCK_SIZE;
        uint64_t buffer_slot_index;

        uint64_t start_pulse_id;
        uint64_t stop_pulse_id;
        uint16_t module_id;

        uint64_t pulse_id[STREAM_BLOCK_SIZE];
        uint64_t frame_index[STREAM_BLOCK_SIZE];
        uint32_t daq_rec[STREAM_BLOCK_SIZE];
        uint16_t n_received_packets[STREAM_BLOCK_SIZE];
    };
    #pragma pack(pop)

    std::string get_filename(
            std::string root_folder,
            std::string device_name,
            uint64_t pulse_id);

    std::size_t get_file_frame_index(uint64_t pulse_id);

    void update_latest_file(
            const std::string& latest_filename,
            const std::string& filename_to_write);

    std::string get_latest_file(const std::string& latest_filename);

    struct path_sufix {
        uint64_t start_pulse_id;
        uint64_t stop_pulse_id;
        std::string path;
    };

    std::vector<path_sufix> get_path_suffixes(
            const uint64_t start_pulse_id,
            const uint64_t stop_pulse_id);
}

#endif //BUFFER_UTILS_HPP
