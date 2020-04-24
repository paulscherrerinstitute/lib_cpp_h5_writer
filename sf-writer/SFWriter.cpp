#include "SFWriter.hpp"
#include "buffer_config.hpp"

using namespace std;
using namespace core_buffer;

SFWriter::SFWriter(
        const string& output_file,
        const size_t n_frames,
        const size_t n_modules)
{
    file_ = H5::H5File(output_file, H5F_ACC_TRUNC);

    hsize_t image_dataset_dims[3] =
            {n_frames, n_modules * MODULE_Y_SIZE, MODULE_X_SIZE};

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

    hsize_t metadata_dataset_dims[2] = {n_frames, 1};
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

//void write(char* data, std::shared_ptr<DetectorFrame> metadata);
//void close_file();