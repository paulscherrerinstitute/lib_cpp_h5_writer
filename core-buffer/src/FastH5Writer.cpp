#include <BufferUtils.hpp>
#include "FastH5Writer.hpp"
#include "date.h"
#include <chrono>
#include <sstream>

extern "C"
{
    #include "H5DOpublic.h"
}

using namespace std;

FastH5Writer::FastH5Writer(
        const size_t n_frames_per_file,
        const uint16_t y_frame_size,
        const uint16_t x_frame_size,
        const string& device_name,
        const string& root_folder) :
            n_frames_per_file_(n_frames_per_file),
            y_frame_size_(y_frame_size),
            x_frame_size_(y_frame_size),
            device_name_(device_name),
            root_folder_(root_folder),
            frame_bytes_size_(2 * y_frame_size * y_frame_size),
            current_output_filename_(""),
            current_output_file_(),
            current_image_dataset_(),
            current_pulse_id_(0)
{
//    // Each element in uint16_t has 2 bytes.
//    size_t n_bytes = 2 * y_frame_size * x_frame_size;
//    auto file = H5::H5File(target_filename.c_str(), H5F_ACC_TRUNC);
}

void FastH5Writer::create_file(const string& filename)
{
    auto file = H5::H5File(filename.c_str(), H5F_ACC_TRUNC);

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

    current_image_dataset_ = current_output_file_.createDataSet(
            "image",
            H5::PredType::NATIVE_UINT16,
            dataspace,
            dataset_properties);

    for (auto& metadata:scalar_metadata_) {
        auto dataset_name = metadata.first;
        auto dataset_type = metadata.second;

        hsize_t dataset_dimension[2] = {n_frames_per_file_, 1};
        H5::DataSpace dataspace(2, dataset_dimension);
        auto dataset = current_output_file_.createDataSet(
                dataset_name,
                dataset_type,
                dataspace);
        datasets_.insert({dataset_name, dataset});

        size_t n_buffer_bytes =
                dataset.getDataType().getSize() * n_frames_per_file_;
        buffers_.insert({dataset_name, make_shared<char[]>(n_buffer_bytes)});
    }
}

void FastH5Writer::close_file()
{
    current_output_filename_ = "";
    current_output_file_.close();
    current_image_dataset_.close();
    current_pulse_id_ = 0;
    current_frame_index_ = 0;

    for (auto& dataset:datasets_) {
        dataset.second.close();
    }
    datasets_.clear();

    buffers_.clear();
}

void FastH5Writer::set_pulse_id(const uint64_t pulse_id)
{
    current_pulse_id_ = pulse_id;
    current_frame_index_ = BufferUtils::get_file_frame_index(pulse_id);

    auto filename = BufferUtils::get_filename(
            root_folder_, device_name_, pulse_id);

    if (filename != current_output_filename_){
        if (current_output_file_.getId() != -1) {
            flush_metadata();
            close_file();
        }
    }

    create_file(filename);
}

void FastH5Writer::flush_metadata()
{
    // TODO: Actually flush this metadata.
}

void FastH5Writer::write_data(const char *buffer)
{
        hsize_t offset[3] = {current_frame_index_, 0, 0};

        if(H5DOwrite_chunk(
                current_image_dataset_.getId(),
                H5P_DEFAULT,
                0, // Filters
                offset, // Offset
                frame_bytes_size_,
                buffer))
        {
            stringstream err_msg;

            using namespace date;
            using namespace chrono;
            err_msg << "[" << system_clock::now() << "]";
            err_msg << "[FastH5Writer::write_data]";
            // TODO: This error message is bad. Extract the real error from lib.
            err_msg << " Error when writing to ";
            err_msg << current_output_filename_;

            throw runtime_error(err_msg.str());
        }
}

template <>
void FastH5Writer::add_scalar_metadata<uint64_t>(
        const std::string& metadata_name)
{
    scalar_metadata_.insert({metadata_name, H5::PredType::NATIVE_UINT64});
}

template <>
void FastH5Writer::add_scalar_metadata<uint32_t>(
        const std::string& metadata_name)
{
    scalar_metadata_.insert({metadata_name, H5::PredType::NATIVE_UINT32});
}

template <>
void FastH5Writer::add_scalar_metadata<uint16_t>(
        const std::string& metadata_name)
{
    scalar_metadata_.insert({metadata_name, H5::PredType::NATIVE_UINT16});
}