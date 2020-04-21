//#include <ZmqRecvModule.hpp>
//#include "BufferMultiReader.hpp"
//#include "BufferUtils.hpp"
//#include <sstream>
//
//using namespace std;
//
//BufferMultiReader::BufferMultiReader(
//        const std::string& root_folder) :
//            root_folder_(root_folder),
//            is_running_(true),
//            pulse_id_(0)
//{
//    frame_buffer_ = new uint16_t[1000*32*512*1024];
//    frame_metadata_buffer_ = new UdpFrameMetadata[32];
//}
//
//BufferMultiReader::~BufferMultiReader()
//{
//    delete[] frame_buffer_;
//    delete[] frame_metadata_buffer_;
//}
//
//char* BufferMultiReader::get_buffer()
//{
//    return (char*) frame_buffer_;
//}
//
//
//UdpFrameMetadata BufferMultiReader::load_frame_to_buffer(
//        const uint64_t pulse_id)
//{
//    if (pulse_id >= min_cached_pulse_id_ && pulse_id <= max_cached_pulse_id_) {
//        auto file_frame_index = BufferUtils::get_file_frame_index(pulse_id);
//        return frame_metadata_buffer_[file_frame_index];
//    }
//
//    for (size_t i_module; i_module<32; i_module++) {
//        stringstream name;
//        name << "M";
//        if (i_module < 10) name << "0";
//        name << (int) i_module;
//        string device_name = name.str();
//
//        auto pulse_filename = BufferUtils::get_filename(
//                root_folder_, device_name, pulse_id);
//
//        H5::H5File input_file(pulse_filename,
//                              H5F_ACC_RDONLY |  H5F_ACC_SWMR_READ);
//
//        auto image_dataset = input_file.openDataSet("image");
//        auto pulse_id_dataset = input_file.openDataSet("pulse_id");
//        auto frame_id_dataset = input_file.openDataSet("frame_id");
//        auto daq_rec_dataset = input_file.openDataSet("daq_rec");
//        auto received_packets_dataset =
//                input_file.openDataSet("received_packets");
//
//        hsize_t buff_dim[3] = {1000, 32*512, 1024};
//        H5::DataSpace buffer_space (3, buff_dim);
//        hsize_t b_offset[] = {0, i_module*512, 0};
//        hsize_t b_stride[] = {n_new_pulses, 512, 1024};
//        hsize_t b_block[] = {1, 512, 1024};
//        hsize_t b_count[] = {1000, 1, 1};
//        buffer_space.selectHyperslab(
//                H5S_SELECT_SET, b_count, b_offset, b_stride, b_block);
//
//        image_dataset.read(
//                image_buffer_, H5::PredType::NATIVE_UINT16,
//                buffer_space);
//        pulse_id_dataset.read(
//                pulse_id_buffer, H5::PredType::NATIVE_UINT64);
//        frame_id_dataset.read(
//                frame_id_buffer, H5::PredType::NATIVE_UINT64);
//        daq_rec_dataset.read(
//                daq_rec_buffer, H5::PredType::NATIVE_UINT32);
//        received_packets_dataset.read(
//                received_packets_buffer, H5::PredType::NATIVE_UINT16);
//    }
//
//    UdpFrameMetadata metadata = frame_metadata_buffer_[0];
//    return metadata;
//}
