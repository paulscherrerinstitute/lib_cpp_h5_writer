#include "FastH5Writer.hpp"


template<>
FastH5Writer<uint16_t>::FastH5Writer(
        const size_t n_frames_per_file,
        const uint16_t y_frame_size,
        const uint16_t x_frame_size) :
            n_frames_per_file_(n_frames_per_file),
            y_frame_size_(y_frame_size),
            x_frame_size_(y_frame_size),
            current_output_filename_(""),
            current_output_file_(),
            current_image_dataset_(),
            current_pulse_id_(0)
{
//    // Each element in uint16_t has 2 bytes.
//    size_t n_bytes = 2 * y_frame_size * x_frame_size;
//    auto file = H5::H5File(target_filename.c_str(), H5F_ACC_TRUNC);
}

template <class B>
void FastH5Writer<B>::create_image_dataset()
{
    hsize_t dataset_dimension[3]  =
            {n_frames_per_file_, y_frame_size_, x_frame_size_};
    hsize_t max_dataset_dimension[3] =
            {n_frames_per_file_, y_frame_size_, x_frame_size_};
    H5::DataSpace dataspace(
            3, dataset_dimension, max_dataset_dimension);

    hsize_t dataset_chunking[3] =
            {CHUNKING_FACTOR, y_frame_size_, x_frame_size_};
    H5::DSetCreatPropList dataset_properties;
    dataset_properties.setChunk(3, dataset_chunking);

    H5::AtomType dataset_data_type(H5::PredType::NATIVE_UINT16);
    dataset_data_type.setOrder(H5T_ORDER_LE);

    current_image_dataset_ = current_output_file_.createDataSet(
            "image", dataset_data_type, dataspace, dataset_properties);
}

template <class B>
void FastH5Writer<B>::set_pulse_id(const uint64_t pulse_id)
{
    current_pulse_id_ = pulse_id;
}

template <class B>
void FastH5Writer<B>::write_data(const char *buffer)
{
    
}