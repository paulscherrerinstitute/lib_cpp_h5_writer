//#ifndef BUFFERMULTIREADER_H
//#define BUFFERMULTIREADER_H
//
//#include <thread>
//#include "RingBuffer.hpp"
//
//
//#pragma pack(push)
//#pragma pack(1)
//struct BufferMetadata {
//    uint64_t pulse_id;
//    uint64_t frame_id;
//    uint32_t daq_rac;
//    uint16_t n_recv_frames;
//};
//#pragma pack(pop)
//
//class BufferMultiReader
//{
//    const std::string root_folder_;
//    std::atomic_bool is_running_;
//    uint16_t* frame_buffer_;
//    UdpFrameMetadata* frame_metadata_buffer_;
//
//    uint64_t min_cached_pulse_id_;
//    uint64_t max_cached_pulse_id_;
//
//
//
//protected:
//    void read_thread(uint8_t module_number);
//
//public:
//    BufferMultiReader(const std::string& root_folder);
//
//    virtual ~BufferMultiReader();
//
//    char* get_buffer();
//
//    UdpFrameMetadata load_frame_to_buffer(const uint64_t pulse_id);
//};
//
//#endif