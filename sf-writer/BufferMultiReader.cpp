#include <ZmqRecvModule.hpp>
#include "BufferMultiReader.hpp"
#include "BufferUtils.hpp"
#include <sstream>

using namespace std;

BufferMultiReader::BufferMultiReader(
        const std::string& root_folder) :
            root_folder_(root_folder),
            is_running_(true),
            pulse_id_(0)
{
    frame_buffer_ = new uint16_t[32*512*1024];
    frame_metadata_buffer_ = new UdpFrameMetadata[32];

    for (uint8_t i_reader=0; i_reader < 32; i_reader++) {
        receiving_threads_.emplace_back(
                &BufferMultiReader::read_thread, this, i_reader);
    }
}

BufferMultiReader::~BufferMultiReader()
{
    is_running_ = false;

    this_thread::sleep_for(chrono::milliseconds(100));

    for (auto& recv_thread:receiving_threads_) {
        if (recv_thread.joinable()) {
            recv_thread.join();
        }
    }

    delete[] frame_buffer_;
    delete[] frame_metadata_buffer_;
}

char* BufferMultiReader::get_buffer()
{
    return (char*) frame_buffer_;
}


UdpFrameMetadata BufferMultiReader::load_frame_to_buffer(
        const uint64_t pulse_id)
{
    memset(frame_buffer_, 0, 2*32*512*1024);
    memset(frame_metadata_buffer_, 0, 32*sizeof(UdpFrameMetadata));

    pulse_id_ = pulse_id;
    n_modules_left_ = 32;

    while (n_modules_left_ > 0) {
        this_thread::sleep_for(chrono::milliseconds(5));
    }

    UdpFrameMetadata metadata = frame_metadata_buffer_[0];
    return metadata;
}

void BufferMultiReader::read_thread(uint8_t module_number)
{
    stringstream name;
    name << "M";
    if (module_number < 10) name << "0";
    name << (int) module_number;

    string device_name = name.str();
    size_t buffer_offset = 512*1024*module_number;

    string current_filename = "";
    uint64_t last_pulse_id = 0;

    auto image_buffer = new uint16_t[1000*512*1024];
    auto pulse_id_buffer = new uint64_t[1000];
    auto frame_id_buffer = new uint64_t[1000];
    auto daq_rec_buffer = new uint32_t[1000];
    auto received_packets_buffer = new uint16_t[1000];

    while (is_running_) {
        if (last_pulse_id == pulse_id_) {
            this_thread::sleep_for(chrono::milliseconds(1));
            continue;
        }
        last_pulse_id = pulse_id_;

        auto pulse_filename = BufferUtils::get_filename(
                root_folder_, device_name, last_pulse_id);

        if (pulse_filename != current_filename) {

            H5::H5File input_file(pulse_filename,
                    H5F_ACC_RDONLY |  H5F_ACC_SWMR_READ);


            auto image_dataset = input_file.openDataSet("image");
            auto pulse_id_dataset = input_file.openDataSet("pulse_id");
            auto frame_id_dataset = input_file.openDataSet("frame_id");
            auto daq_rec_dataset = input_file.openDataSet("daq_rec");
            auto received_packets_dataset =
                    input_file.openDataSet("received_packets");

            image_dataset.read(
                    image_buffer, H5::PredType::NATIVE_UINT16);
            pulse_id_dataset.read(
                    pulse_id_buffer, H5::PredType::NATIVE_UINT64);
            frame_id_dataset.read(
                    frame_id_buffer, H5::PredType::NATIVE_UINT64);
            daq_rec_dataset.read(
                    daq_rec_buffer, H5::PredType::NATIVE_UINT32);
            received_packets_dataset.read(
                    received_packets_buffer, H5::PredType::NATIVE_UINT16);

            current_filename = pulse_filename;
        }

        auto file_frame_index =
                BufferUtils::get_file_frame_index(last_pulse_id);

        memcpy(
                (char*) (frame_buffer_ + buffer_offset),
                (char*)(image_buffer + (file_frame_index*512*1024)),
                512*1024*2);

        UdpFrameMetadata metadata;
        metadata.pulse_id = pulse_id_buffer[file_frame_index];
        metadata.frame_index = frame_id_buffer[file_frame_index];
        metadata.daq_rec = daq_rec_buffer[file_frame_index];
        metadata.n_recv_packets = received_packets_buffer[file_frame_index];

        memcpy(
                (char*) (frame_metadata_buffer_ + module_number),
                &metadata,
                sizeof(UdpFrameMetadata));

        n_modules_left_--;
    }
}
