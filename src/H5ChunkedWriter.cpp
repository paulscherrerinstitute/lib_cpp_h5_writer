#include <H5Cpp.h>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "H5ChunkedWriter.hpp"
#include "h5_file_format.hpp"

extern "C"
{
    #include "H5DOpublic.h"
}

using namespace std;

hsize_t expand_dataset(const H5::DataSet& dataset, hsize_t frame_index, hsize_t dataset_increase_step)
{
    hsize_t dataset_rank = 3;
    hsize_t dataset_dimension[dataset_rank];

    dataset.getSpace().getSimpleExtentDims(dataset_dimension);
    dataset_dimension[0] = frame_index + dataset_increase_step;

    #ifdef DEBUG_OUTPUT
        cout << "[expand_dataset] Expanding dataspace to size (";
        for (hsize_t i=0; i<dataset_rank; ++i) {
            cout << dataset_dimension[i] << ",";
        }
        cout << ")" << endl;
    #endif

    dataset.extend(dataset_dimension);

    return dataset_dimension[0];
}

void compact_dataset(const H5::DataSet& dataset, hsize_t max_frame_index)
{
    hsize_t dataset_rank = 3;
    hsize_t dataset_dimension[dataset_rank];

    dataset.getSpace().getSimpleExtentDims(dataset_dimension);
    dataset_dimension[0] = max_frame_index + 1;

    #ifdef DEBUG_OUTPUT
        cout << "[compact_dataset] Compacting dataspace to size (";
        for (hsize_t i=0; i<dataset_rank; ++i) {
            cout << dataset_dimension[i] << ",";
        }
        cout << ")" << endl;
    #endif

    dataset.extend(dataset_dimension);
}

HDF5ChunkedWriter::HDF5ChunkedWriter(const string filename, const string dataset_name, hsize_t frames_per_file, hsize_t initial_dataset_size) :
    filename(filename), dataset_name(dataset_name), frames_per_file(frames_per_file), initial_dataset_size(initial_dataset_size)
{
    #ifdef DEBUG_OUTPUT
        cout << "[HDF5ChunkedWriter::HDF5ChunkedWriter] Creating chunked writer"; 
        cout << " with filename " << filename;
        cout << " and dataset_name " << dataset_name;
        cout << " and frames_per_file " << frames_per_file;
        cout << " and initial_dataset_size " << initial_dataset_size;
        cout << endl;
    #endif
}

HDF5ChunkedWriter::~HDF5ChunkedWriter()
{
    close_file();
}

void HDF5ChunkedWriter::close_file()
{
    if (file.getId() == -1) {
        #ifdef DEBUG_OUTPUT
            cout << "[HDF5ChunkedWriter::close_file] Trying to close an already closed file." << endl;
        #endif

        return;
    }

    #ifdef DEBUG_OUTPUT
        cout << "[HDF5ChunkedWriter::close_file] Closing file." << endl;
    #endif

    compact_dataset(dataset, max_frame_index);
    
    hsize_t min_frame_in_dataset = 0;
    if (frames_per_file) {
        min_frame_in_dataset = (current_frame_chunk - 1) * frames_per_file;
    }

    // max_frame_index is relative to the current file.
    hsize_t max_frame_in_dataset = max_frame_index + min_frame_in_dataset;

    // Frame indexing starts at 1 (for some reason).
    auto image_nr_low = min_frame_in_dataset + 1;
    auto image_nr_high = max_frame_in_dataset + 1;

    #ifdef DEBUG_OUTPUT
        cout << "[HDF5ChunkedWriter::close_file] Setting dataset attribute image_nr_low=" << image_nr_low << " and image_nr_high=" << image_nr_high << endl;
    #endif

    h5_utils::write_attribute(dataset, "image_nr_low", image_nr_low);
    h5_utils::write_attribute(dataset, "image_nr_high", image_nr_high);

    // Cleanup.
    file.close();
    current_frame_chunk = 0;
    current_dataset_size = 0;
    max_frame_index = 0;
}

void HDF5ChunkedWriter::write_data(size_t frame_index, size_t* frame_shape, size_t data_bytes_size, char* data)
{
    // Define the ofset of the currently received image in the file.
    hsize_t relative_frame_index = prepare_storage_for_frame(frame_index, frame_shape);

    // Define where to write values in the dataset.
    const hsize_t offset[] = {relative_frame_index, 0, 0};
    uint32_t filters = 0;
    
    if( H5DOwrite_chunk(dataset.getId(), H5P_DEFAULT, filters, offset, data_bytes_size, data) )
    {
        stringstream error_message;
        error_message << "Error while writing chunk to file at offset " << relative_frame_index << "." << endl;

        throw invalid_argument( error_message.str() );
    }
}

void HDF5ChunkedWriter::create_file(size_t* frame_shape, hsize_t frame_chunk) {

    if (file.getId() != -1) {
        close_file();
    }

    auto target_filename = filename;

    // In case frames_per_file is > 0, the filename variable is a template for the filename.
    if (frames_per_file) {
        #ifdef DEBUG_OUTPUT
            cout << "[HDF5ChunkedWriter::create_file] Frames per file is defined. Format " << filename << " with frame_chunk " << frame_chunk << endl;
        #endif

        // Space for 10 digits should be enough.
        char buffer[filename.length() + 10];

        sprintf(buffer, filename.c_str(), frame_chunk);
        target_filename = string(buffer);
    }

    #ifdef DEBUG_OUTPUT
        cout << "[HDF5ChunkedWriter::create_file] Creating filename " << target_filename << endl;
    #endif

    // TODO: Create folder if it does not exist.

    file = H5::H5File( target_filename.c_str(), H5F_ACC_TRUNC );
    
    H5::IntType data_type( config::dataset_type );
    data_type.setOrder( config::dataset_byte_order );

    hsize_t dataset_rank = 3;
    const hsize_t dataset_dimension[] = {initial_dataset_size, frame_shape[0], frame_shape[1]};
    const hsize_t max_dataset_dimension[] = {H5S_UNLIMITED, frame_shape[0], frame_shape[1]};
    H5::DataSpace dataspace(dataset_rank, dataset_dimension, max_dataset_dimension);

    #ifdef DEBUG_OUTPUT
        cout << "[HDF5ChunkedWriter::create_file] Creating dataspace of size (";
        for (hsize_t i=0; i<dataset_rank; ++i) {
            cout << dataset_dimension[i] << ",";
        }
        cout << ")" << endl;
    #endif

    // Set chunking to single image.
    H5::DSetCreatPropList dataset_properties;
    const hsize_t dataset_chunking[] = {1, frame_shape[0], frame_shape[1]};
    dataset_properties.setChunk(dataset_rank, dataset_chunking);

    // Take into account initial size, set chunking.
    dataset = file.createDataSet(dataset_name.c_str(), data_type, dataspace, dataset_properties);

    // New file created - update global values.
    current_frame_chunk = frame_chunk;
    current_dataset_size = initial_dataset_size;

}

hsize_t HDF5ChunkedWriter::prepare_storage_for_frame(size_t frame_index, size_t* frame_shape) {

    hsize_t relative_frame_index = frame_index;

    // Check if we have to create a new file.
    if (frames_per_file) {
        hsize_t frame_chunk = (frame_index / frames_per_file) + 1;

        // This frames does not go into this file.
        if (frame_chunk != current_frame_chunk) {
            create_file(frame_shape, frame_chunk);
        }

        // Make the frame_index relative to this chunk (file).
        relative_frame_index = frame_index - ((frame_chunk - 1) * frames_per_file);
    }

    #ifdef DEBUG_OUTPUT
        cout << "[HDF5ChunkedWriter::prepare_storage_for_frame] Received frame index " << frame_index << " and processed as relative frame index " << relative_frame_index << endl;
    #endif

    // Open the file if needed.
    if (file.getId() == -1) {
        create_file(frame_shape);
    }

    // Expand the dataset if needed.
    if (relative_frame_index > current_dataset_size) {
        current_dataset_size = expand_dataset(dataset, relative_frame_index, config::dataset_increase_step);
    }

    // Keep track of the max index in this file - needed for shrinking the dataset at the end.
    if (relative_frame_index > max_frame_index) {
        max_frame_index = relative_frame_index;
    }

    return relative_frame_index;
}

void HDF5ChunkedWriter::write_format(h5_base& format_root, std::map<std::string, h5_value>& values) {
}