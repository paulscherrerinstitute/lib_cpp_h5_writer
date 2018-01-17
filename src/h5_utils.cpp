#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "h5_utils.hpp"

using namespace std;

hsize_t h5_utils::expand_dataset(const H5::DataSet& dataset, hsize_t frame_index, hsize_t dataset_increase_step)
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

void h5_utils::compact_dataset(const H5::DataSet& dataset, hsize_t max_frame_index)
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

H5::Group h5_utils::create_group(H5::CommonFG& target, std::string name)
{
    return target.createGroup(name);
}

boost::any h5_utils::get_value_from_reference(string& dataset_name, boost::any value_reference, map<string, boost::any>& values)
{
    try {
        auto reference_string = boost::any_cast<string>(value_reference);
        return values.at(reference_string);

    } catch (const boost::bad_any_cast& exception) {
        stringstream error_message;
        error_message << "Cannot convert dataset " << dataset_name << " value reference to string." << endl;

        throw runtime_error(error_message.str());

    } catch (const out_of_range& exception){
        stringstream error_message;
        error_message << "Dataset " << dataset_name << " value reference " << boost::any_cast<string>(value_reference) << " not present in values map." << endl;

        throw runtime_error(error_message.str());
    }
}

H5::DataSet h5_utils::write_dataset(H5::Group& target, h5_dataset& dataset, map<string, boost::any>& values){
    string name = dataset.name;
    boost::any value;

    // Value is stored directly in the struct.
    if (dataset.data_location == IMMEDIATE){
        value = dataset.value;
    // Value in struct is just a string reference to into the values map.
    } else {
        value = h5_utils::get_value_from_reference(name, dataset.value, values);
    }
    
    if (dataset.data_type == NX_CHAR || dataset.data_type == NX_DATE_TIME || dataset.data_type == NXnote) {
        // Attempt to convert to const char * (string "literals" cause that).
        try {
            return h5_utils::write_dataset(target, name, string(boost::any_cast<const char*>(value)));
        } catch (const boost::bad_any_cast& exception) {}

        // Atempt to convert to string.
        try {
            return h5_utils::write_dataset(target, name, boost::any_cast<string>(value));
        } catch (const boost::bad_any_cast& exception) {}

        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "Cannot convert dataset " << name << " to string or const char*." << endl;

        throw runtime_error(error_message.str());

    } else if (dataset.data_type == NX_INT) {
        try {
            return h5_utils::write_dataset(target, name, boost::any_cast<int>(value));
        } catch (const boost::bad_any_cast& exception) {}

        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "Cannot convert dataset " << name << " to NX_INT." << endl;

        throw runtime_error(error_message.str());
    } else if (dataset.data_type == NX_FLOAT || dataset.data_type == NX_NUMBER) {
        try {
            return h5_utils::write_dataset(target, name, boost::any_cast<double>(value));
        } catch (const boost::bad_any_cast& exception) {}

        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "Cannot convert dataset " << name << " to NX_FLOAT." << endl;

        throw runtime_error(error_message.str());
    } else {
        stringstream error_message;
        error_message << "Unsupported dataset type for dataset " << name << "." << endl;

        throw runtime_error(error_message.str());
    }
}

H5::DataSet h5_utils::write_dataset(H5::Group& target, string name, double value)
{
    H5::DataSpace att_space(H5S_SCALAR);
    auto data_type = H5::PredType::NATIVE_DOUBLE;

    H5::DataSet dataset = target.createDataSet(name, data_type , att_space);
    dataset.write(&value, data_type);

    return dataset;
}

H5::DataSet h5_utils::write_dataset(H5::Group& target, string name, int value)
{
    H5::DataSpace att_space(H5S_SCALAR);
    auto data_type = H5::PredType::NATIVE_INT;

    H5::DataSet dataset = target.createDataSet(name, data_type, att_space);
    dataset.write(&value, data_type);

    return dataset;
}

H5::DataSet h5_utils::write_dataset(H5::Group& target, string name, string value)
{
    H5::DataSpace att_space(H5S_SCALAR);
    H5::DataType data_type = H5::StrType(0, H5T_VARIABLE);

    H5::DataSet dataset = target.createDataSet(name, data_type ,att_space);
    dataset.write(&value, data_type);

    return dataset;
}

void h5_utils::write_attribute(H5::H5Object& target, string name, string value)
{
    H5::DataSpace att_space(H5S_SCALAR);
    H5::DataType data_type = H5::StrType(0, H5T_VARIABLE);

    auto h5_attribute = target.createAttribute(name, data_type, att_space);
    h5_attribute.write(data_type, value);
}

void h5_utils::write_attribute(H5::H5Object& target, string name, int value)
{
    H5::DataSpace att_space(H5S_SCALAR);
    auto data_type = H5::PredType::NATIVE_INT;

    auto h5_attribute = target.createAttribute(name, data_type, att_space);
    h5_attribute.write(data_type, &value);
}

void h5_utils::write_attribute(H5::H5Object& target, h5_attr& attribute, map<string, boost::any>& values) 
{
    string name = attribute.name;
    boost::any value;

    // Value is stored directly in the struct.
    if (attribute.data_location == IMMEDIATE){
        value = attribute.value;
    // Value in struct is just a string reference to into the values map.
    } else {
        value = h5_utils::get_value_from_reference(name, attribute.value, values);
    }
    
    if (attribute.data_type == NX_CHAR) {
        // Attempt to convert to const char * (string "literals" cause that).
        try {
            h5_utils::write_attribute(target, name, string(boost::any_cast<const char*>(value)));
            return;
        } catch (const boost::bad_any_cast& exception) {}

        // Atempt to convert to string.
        try {
            h5_utils::write_attribute(target, name, boost::any_cast<string>(value));
            return;
        } catch (const boost::bad_any_cast& exception) {}

        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "Cannot convert attribute " << name << " to string or const char*." << endl;

        throw runtime_error(error_message.str());

    } else if (attribute.data_type == NX_INT) {
        try {
            h5_utils::write_attribute(target, name, boost::any_cast<int>(value));
            return;
        } catch (const boost::bad_any_cast& exception) {}

        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "Cannot convert attribute " << name << " to INT." << endl;

        throw runtime_error(error_message.str());
    }
}

void h5_utils::write_format_data(H5::CommonFG& file_node, h5_parent& format_node, std::map<std::string, h5_value>& values) {
    auto node_group = h5_utils::create_group(file_node, format_node.name);

    for (auto item : format_node.items) {

        if (item->node_type == GROUP) {
            auto sub_group = dynamic_cast<h5_group*>(item); 

            write_format_data(node_group, *sub_group, values);
        } else if (item->node_type == ATTRIBUTE) {
            auto sub_attribute = dynamic_cast<h5_attr*>(item);

            h5_utils::write_attribute(node_group, *sub_attribute, values);
        } else if (item->node_type == DATASET) {
            auto sub_dataset = dynamic_cast<h5_dataset*>(item);
            auto current_dataset = h5_utils::write_dataset(node_group, *sub_dataset, values);

            for (auto dataset_attr : sub_dataset->items) {

                // You can specify only attributes inside a dataset.
                if (dataset_attr->node_type != ATTRIBUTE) {
                    stringstream error_message;
                    error_message << "Invalid element " << dataset_attr->name << " on dataset " << sub_dataset->name << ". Only attributes allowd.";

                    throw invalid_argument( error_message.str() );
                }

                auto sub_attribute = dynamic_cast<h5_attr*>(dataset_attr);

                h5_utils::write_attribute(current_dataset, *sub_attribute, values);
            }
        }
    }
}

void h5_utils::write_format(H5::H5File& file, std::map<std::string, h5_value>& input_values){
    
    auto format = get_format_definition();
    auto values = get_default_values();
    
    add_input_values(*values, input_values);
    add_calculated_values(*values);
    
    write_format_data(file, *format, *values);
}