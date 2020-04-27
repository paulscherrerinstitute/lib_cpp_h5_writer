#include "SFWriter.hpp"
#include "buffer_config.hpp"

extern "C"
{
    #include "H5DOpublic.h"
}

using namespace std;
using namespace core_buffer;

SFWriter::SFWriter(
        const string& output_file,
        const size_t n_frames,
        const size_t n_modules) :
        n_frames_(n_frames),
        n_modules_(n_modules),
        current_write_index_(0),
        image_buffer_count_(0)
{
    file_ = H5::H5File(output_file, H5F_ACC_TRUNC);

    hsize_t image_dataset_dims[3] =
            {n_frames_, n_modules * MODULE_Y_SIZE, MODULE_X_SIZE};

    H5::DataSpace image_dataspace(3, image_dataset_dims);

    hsize_t image_dataset_chunking[3] =
            {1, n_modules * MODULE_Y_SIZE, MODULE_X_SIZE};
    H5::DSetCreatPropList image_dataset_properties;
    image_dataset_properties.setChunk(3, image_dataset_chunking);

    image_dataset_ = file_.createDataSet(
            "image",
            H5::PredType::NATIVE_UINT16,
            image_dataspace,
            image_dataset_properties);

    hsize_t metadata_dataset_dims[2] = {n_frames_, 1};
    H5::DataSpace metadata_dataspace(2, metadata_dataset_dims);

    pulse_id_dataset_ = file_.createDataSet(
            "pulse_id",
            H5::PredType::NATIVE_UINT64,
            metadata_dataspace);

    pulse_id_dataset_ = file_.createDataSet(
            "frame_index",
            H5::PredType::NATIVE_UINT64,
            metadata_dataspace);

    pulse_id_dataset_ = file_.createDataSet(
            "daq_rec",
            H5::PredType::NATIVE_UINT32,
            metadata_dataspace);

    pulse_id_dataset_ = file_.createDataSet(
            "n_received_packets",
            H5::PredType::NATIVE_UINT16,
            metadata_dataspace);

    image_buffer_ = make_unique<char[]>(
            n_modules_ * MODULE_N_BYTES * WRITER_BUFFER_SIZE);
    image_buffer_count_ = 0;
}

SFWriter::~SFWriter()
{
    close_file();
}

void SFWriter::close_file()
{
    image_dataset_.close();
    pulse_id_dataset_.close();
    frame_index_dataset_.close();
    daq_rec_dataset_.close();
    n_received_packets_dataset_.close();

    file_.close();
}

void SFWriter::write(shared_ptr<DetectorFrame> metadata, char* data) {
    auto pulse_id = metadata->pulse_id;
    auto frame_index = metadata->frame_index;
    auto daq_rec = metadata->daq_rec;
    auto n_received_packets = metadata->n_received_packets;

//    hsize_t buff_dim[2] = {n_modules_*MODULE_Y_SIZE, MODULE_X_SIZE};
//    H5::DataSpace buffer_space (2, buff_dim);
//
//    hsize_t disk_dim[3] = {n_frames_, n_modules_*MODULE_Y_SIZE, MODULE_X_SIZE};
//    H5::DataSpace disk_space(3, disk_dim);
//
//    hsize_t count[] = {1, n_modules_*MODULE_Y_SIZE, MODULE_X_SIZE};
//    hsize_t start[] = {current_write_index_, 0, 0};
//    disk_space.selectHyperslab(H5S_SELECT_SET, count, start);
//
//    image_dataset_.write(data, H5::PredType::NATIVE_UINT16,
//            buffer_space,
//            disk_space);

    if (image_buffer_count_ < WRITER_BUFFER_SIZE) {
        char* buffer = image_buffer_.get();

        memcpy(
                (buffer + image_buffer_count_),
                data,
                MODULE_N_BYTES * n_modules_);

        image_buffer_count_++;
    } else {
        hsize_t offset[] = {current_write_index_, 0, 0};

        if( H5DOwrite_chunk(
                image_dataset_.getId(),
                H5P_DEFAULT,
                0,
                offset,
                MODULE_N_BYTES * n_modules_ * WRITER_BUFFER_SIZE,
                image_buffer_.get()))
        {
            stringstream error_message;
            using namespace date;
            error_message << "[" << std::chrono::system_clock::now() << "]";
            error_message << "Error while writing data to file at offset ";
            error_message << current_write_index_ << "." << endl;

            throw runtime_error(error_message.str());
        }

        current_write_index_ += WRITER_BUFFER_SIZE;
        image_buffer_count_ = 0;
    }
}
