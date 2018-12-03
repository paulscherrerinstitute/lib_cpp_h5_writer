#include <sstream>
#include <stdexcept>
#include <iostream>

#include "H5Writer.hpp"
#include "H5Format.hpp"

extern "C"
{
    #include "H5DOpublic.h"
}

using namespace std;

std::unique_ptr<H5Writer> get_h5_writer(const string& filename, hsize_t frames_per_file, 
    hsize_t initial_dataset_size, hsize_t dataset_increase_step)
{
    if (filename == "/dev/null") {
        return unique_ptr<H5Writer>(new DummyH5Writer());
    } else {
        return unique_ptr<H5Writer>(new H5Writer(filename, frames_per_file, initial_dataset_size, dataset_increase_step));
    }
}

H5Writer::H5Writer(const std::string& filename, hsize_t frames_per_file, hsize_t initial_dataset_size, 
    hsize_t dataset_increase_step) :
        filename(filename), frames_per_file(frames_per_file), 
        initial_dataset_size(initial_dataset_size), dataset_increase_step(dataset_increase_step)
{
    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[H5Writer::H5Writer] Creating chunked writer"; 
        cout << " with filename " << filename;
        cout << " and frames_per_file " << frames_per_file;
        cout << " and initial_dataset_size " << initial_dataset_size;
        cout << endl;
    #endif
}

H5Writer::~H5Writer()
{
    close_file();
}

void H5Writer::close_file()
{
    if (is_file_open()) {

        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << std::chrono::system_clock::now() << "]";
            cout << "[H5Writer::close_file] Closing file." << endl;
        #endif

        hsize_t min_frame_in_dataset = 0;
        if (frames_per_file) {
            min_frame_in_dataset = (current_frame_chunk - 1) * frames_per_file;
        }

        // max_data_index is relative to the current file.
        hsize_t max_frame_in_dataset = max_data_index + min_frame_in_dataset;

        // Frame indexing starts at 1 (for some reason).
        auto image_nr_low = min_frame_in_dataset + 1;
        auto image_nr_high = max_frame_in_dataset + 1;

        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << std::chrono::system_clock::now() << "]";
            cout << "[H5Writer::close_file] Setting datasets attribute image_nr_low=" << image_nr_low;
            cout << " and image_nr_high=" << image_nr_high << endl;
        #endif

        for (const auto& dataset_map : datasets) {
            auto dataset = dataset_map.second;

            H5FormatUtils::compact_dataset(dataset, max_data_index);

            H5FormatUtils::write_attribute(dataset, "image_nr_low", image_nr_low);
            H5FormatUtils::write_attribute(dataset, "image_nr_high", image_nr_high);
        }

        file.close();

    } else {
        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << std::chrono::system_clock::now() << "]";
            cout << "[H5Writer::close_file] Trying to close an already closed file." << endl;
        #endif
    }

    // Cleanup.
    datasets.clear();
    datasets_current_size.clear();

    current_frame_chunk = 0;
    max_data_index = 0;
}

void H5Writer::write_data(const string& dataset_name, const size_t data_index, const char* data,
    const std::vector<size_t>& data_shape, const size_t data_bytes_size, const string& data_type, const string& endianness)
{
    try {

        // Define the ofset of the currently received image in the file.
        hsize_t relative_data_index = prepare_storage_for_data(dataset_name, data_index, data_shape, data_type, endianness);

        // Define the offset where to write the data.
        size_t data_rank = data_shape.size();
        hsize_t offset[data_rank+1];
        
        offset[0] = relative_data_index;
        for (uint index=0; index<data_rank; ++index) {
            offset[index+1] = 0;
        }

        // No compression for now.
        uint32_t filters = 0;

        const auto& dataset = datasets.at(dataset_name);
        
        if( H5DOwrite_chunk(dataset.getId(), H5P_DEFAULT, filters, offset, data_bytes_size, data) )
        {
            stringstream error_message;
            using namespace date;
            error_message << "[" << std::chrono::system_clock::now() << "]";
            error_message << "Error while writing dataset " << dataset_name << " chunk to file at offset ";
            error_message << relative_data_index << "." << endl;

            throw invalid_argument( error_message.str() );
        }
    } catch (...) {
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[H5Writer::write_data] Error while trying to write data to dataset " << dataset_name << endl; 
        
        throw;
    }
}

void H5Writer::create_dataset(const string& dataset_name, const vector<size_t>& data_shape, 
    const string& data_type, const string& endianness)
{
    // Number of dimensions in each data point.
    const size_t data_rank = data_shape.size();
    // The +1 dimension is to account for the sequence of data points (time).
    const hsize_t dataset_rank = data_rank + 1;

    hsize_t dataset_dimension[dataset_rank];
    hsize_t max_dataset_dimension[dataset_rank];
    hsize_t dataset_chunking[dataset_rank];
    
    dataset_dimension[0] = initial_dataset_size;
    // This dataset can be resized without limits. 
    max_dataset_dimension[0] = H5S_UNLIMITED;
    // Chunking is always set to a single data point.
    dataset_chunking[0] = 1;

    for (size_t index=0; index<data_rank; ++index) {
        dataset_dimension[index+1] = data_shape[index];
        max_dataset_dimension[index+1] = data_shape[index];
        dataset_chunking[index+1] = data_shape[index];
    }

    H5::DataSpace dataspace(dataset_rank, dataset_dimension, max_dataset_dimension);

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[H5Writer::create_dataset] Creating dataspace of size (";
        for (hsize_t i=0; i<dataset_rank; ++i) {
            cout << dataset_dimension[i] << ",";
        }
        cout << ")" << endl;
    #endif

    H5::DSetCreatPropList dataset_properties;
    dataset_properties.setChunk(dataset_rank, dataset_chunking);
    
    H5::AtomType dataset_data_type(H5FormatUtils::get_dataset_data_type(data_type));

    if (endianness == "big") {
        dataset_data_type.setOrder(H5T_ORDER_BE);
    } else {
        dataset_data_type.setOrder(H5T_ORDER_LE);
    }

    auto dataset = file.createDataSet(dataset_name.c_str(), dataset_data_type, dataspace, dataset_properties);
    
    datasets.insert({dataset_name, dataset});
    datasets_current_size.insert({dataset_name, initial_dataset_size});
}

void H5Writer::create_file(hsize_t frame_chunk) 
{

    if (file.getId() != -1) {
        close_file();
    }

    auto target_filename = filename;

    // In case frames_per_file is > 0, the filename variable is a template for the filename.
    if (frames_per_file) {
        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << std::chrono::system_clock::now() << "]";
            cout << "[H5Writer::create_file] Frames per file is defined. Format " << filename << " with frame_chunk " << frame_chunk << endl;
        #endif

        // Space for 10 digits should be enough.
        char buffer[filename.length() + 10];

        sprintf(buffer, filename.c_str(), frame_chunk);
        target_filename = string(buffer);
    }

    #ifdef DEBUG_OUTPUT
        using namespace date;
        cout << "[" << std::chrono::system_clock::now() << "]";
        cout << "[H5Writer::create_file] Creating filename " << target_filename << endl;
    #endif

    file = H5::H5File(target_filename.c_str(), H5F_ACC_TRUNC);

    if (file.getId() == -1) {
       stringstream error_message;
       using namespace date;
       error_message << "[" << std::chrono::system_clock::now() << "]";
       error_message << "Cannot create new file with filename " << target_filename << endl;

       throw runtime_error(error_message.str());
    }
    
    // New file created - set this files chunk number.
    current_frame_chunk = frame_chunk;
}

bool H5Writer::is_file_open() const
{
    return (file.getId() != -1);
}

hsize_t H5Writer::prepare_storage_for_data(const string& dataset_name, const size_t data_index, const std::vector<size_t>& data_shape,
     const string& data_type, const string& endianness) 
{
    hsize_t relative_data_index = data_index;

    // Check if we have to create a new file.
    if (frames_per_file) {
        hsize_t frame_chunk = (data_index / frames_per_file) + 1;

        // This frames does not go into this file.
        if (frame_chunk != current_frame_chunk) {
            create_file(frame_chunk);
        }

        // Make the data_index relative to this chunk (file).
        relative_data_index = data_index - ((frame_chunk - 1) * frames_per_file);
    }

    // Open the file if needed.
    if (!is_file_open()) {
        create_file();
    }

    // Create the dataset if we don't have it yet.
    if (datasets.find(dataset_name) == datasets.end()) {
        create_dataset(dataset_name, data_shape, data_type, endianness);
    }

    hsize_t current_dataset_size = datasets_current_size.at(dataset_name);

    // Expand the dataset if needed.
    if (relative_data_index > current_dataset_size) {
        auto dataset = datasets.at(dataset_name);

        hsize_t new_dataset_size = H5FormatUtils::expand_dataset(dataset, relative_data_index, dataset_increase_step);
        datasets_current_size[dataset_name] = new_dataset_size;
    }

    // Keep track of the max index in this file - needed for shrinking the dataset at the end.
    if (relative_data_index > max_data_index) {
        max_data_index = relative_data_index;
    }

    return relative_data_index;
}

H5::H5File& H5Writer::get_h5_file() 
{
    return file;
}

DummyH5Writer::DummyH5Writer() : H5Writer("/dev/null", 0, 0, 0){}

void DummyH5Writer::close_file(){}

void DummyH5Writer::write_data(const string& dataset_name, const size_t data_index, const char* data, const std::vector<size_t>& data_shape, 
    const size_t data_bytes_size, const string& data_type, const string& endianness){};

bool DummyH5Writer::is_file_open() const
{
    return false;
}

H5::H5File& DummyH5Writer::get_h5_file(){
    stringstream error_message;
    using namespace date;
    error_message << "[" << std::chrono::system_clock::now() << "]";
    error_message << "Cannot get the H5 file with the dummy writer." << endl;

    throw runtime_error(error_message.str());
};

