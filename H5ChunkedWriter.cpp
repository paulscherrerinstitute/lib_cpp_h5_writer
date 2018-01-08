#include "H5ChunkedWriter.hpp"

extern "C"
{
    #include "H5DOpublic.h"
}

hsize_t expand_dataset(const H5::DataSet& dataset, hsize_t frame_index, hsize_t dataset_increase_step)
{
    hsize_t dataset_rank = 3;
    hsize_t dataset_dimension[dataset_rank];

    dataset.getSpace().getSimpleExtentDims(dataset_dimension);
    dataset_dimension[0] = frame_index + dataset_increase_step;

    #ifdef DEBUG
        std::cout << "Expanding dataspace to size (";
        for (hsize_t i=0; i<dataset_rank; ++i) {
            std::cout << dataset_dimension[i] << ",";
        }
        std::cout << ")" << std::endl;
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

    #ifdef DEBUG
        std::cout << "Compacting dataspace to size (";
        for (hsize_t i=0; i<dataset_rank; ++i) {
            std::cout << dataset_dimension[i] << ",";
        }
        std::cout << ")" << std::endl;
    #endif

    dataset.extend(dataset_dimension);
}

HDF5ChunkedWriter::HDF5ChunkedWriter(const std::string filename, const std::string dataset_name, hsize_t frames_per_file, hsize_t initial_dataset_size)
{
    this->filename = filename;
    this->dataset_name = dataset_name;
    this->frames_per_file = frames_per_file;
    this->initial_dataset_size = initial_dataset_size;
}

HDF5ChunkedWriter::~HDF5ChunkedWriter()
{
    close_file();
}

void HDF5ChunkedWriter::close_file()
{
    if (file.getId() == -1) {
        #ifdef DEBUG
            std::cout << "Trying to close an already closed file." << std::endl;
        #endif

        return;
    }

    #ifdef DEBUG
        std::cout << "Closing file." << std::endl;
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

    #ifdef DEBUG
        std::cout << "Setting dataset attribute image_nr_low=" << image_nr_low << " and image_nr_high=" << image_nr_high << std::endl;
    #endif

    // H5::IntType int_type(H5::PredType::NATIVE_UINT32);
    // H5::DataSpace att_space(H5S_SCALAR);
    // auto low_index_attribute = dataset.createAttribute("image_nr_low", int_type, att_space);

    // H5::IntType int_type(H5::PredType::NATIVE_UINT32);
    // H5::DataSpace att_space(H5S_SCALAR);
    // auto high_index_attribute = dataset.createAttribute("image_nr_high", int_type, att_space);

    // TODO: Populate additional h5 attributes.

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
        std::stringstream error_message;
        error_message << "Error while writing chunk to file at offset " << relative_frame_index << "." << std::endl;

        throw std::invalid_argument( error_message.str() );
    }
}

void HDF5ChunkedWriter::create_file(size_t* frame_shape, hsize_t frame_chunk) {

    if (file.getId() != -1) {
        close_file();
    }

    auto target_filename = filename;

    // In case frames_per_file is > 0, the filename variable is a template for the filename.
    if (frames_per_file) {
        #ifdef DEBUG
            std::cout << "Frames per file is defined. Format " << filename << " with frame_chunk " << frame_chunk << std::endl;
        #endif

        // Space for 10 digits should be enough.
        char buffer[filename.length() + 10];

        sprintf(buffer, filename.c_str(), frame_chunk);
        target_filename = std::string(buffer);
    }

    #ifdef DEBUG
        std::cout << "Creating filename " << target_filename << std::endl;
    #endif

    // TODO: Create folder if it does not exist.

    file = H5::H5File( target_filename.c_str(), H5F_ACC_TRUNC );
    
    H5::IntType data_type( config::dataset_type );
    data_type.setOrder( config::dataset_byte_order );

    hsize_t dataset_rank = 3;
    const hsize_t dataset_dimension[] = {initial_dataset_size, frame_shape[0], frame_shape[1]};
    const hsize_t max_dataset_dimension[] = {H5S_UNLIMITED, frame_shape[0], frame_shape[1]};
    H5::DataSpace dataspace(dataset_rank, dataset_dimension, max_dataset_dimension);

    #ifdef DEBUG
        std::cout << "Creating dataspace of size (";
        for (hsize_t i=0; i<dataset_rank; ++i) {
            std::cout << dataset_dimension[i] << ",";
        }
        std::cout << ")" << std::endl;
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

    #ifdef DEBUG
        std::cout << "Received frame index " << frame_index << " and processed as relative frame index " << relative_frame_index << std::endl;
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
