#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "H5Format.hpp"

using namespace std;


hsize_t H5FormatUtils::expand_dataset(H5::DataSet& dataset, hsize_t frame_index, hsize_t dataset_increase_step)
{
    hsize_t dataset_rank = 3;
    hsize_t dataset_dimension[dataset_rank];

    dataset.getSpace().getSimpleExtentDims(dataset_dimension);
    dataset_dimension[0] = frame_index + dataset_increase_step;

    #ifdef DEBUG_OUTPUT
        cout << "[H5FormatUtils::expand_dataset] Expanding dataspace to size (";
        for (hsize_t i=0; i<dataset_rank; ++i) {
            cout << dataset_dimension[i] << ",";
        }
        cout << ")" << endl;
    #endif

    dataset.extend(dataset_dimension);

    return dataset_dimension[0];
}

void H5FormatUtils::compact_dataset(H5::DataSet& dataset, hsize_t max_frame_index)
{
    hsize_t dataset_rank = 3;
    hsize_t dataset_dimension[dataset_rank];

    dataset.getSpace().getSimpleExtentDims(dataset_dimension);
    dataset_dimension[0] = max_frame_index + 1;

    #ifdef DEBUG_OUTPUT
        cout << "[H5FormatUtils::compact_dataset] Compacting dataspace to size (";
        for (hsize_t i=0; i<dataset_rank; ++i) {
            cout << dataset_dimension[i] << ",";
        }
        cout << ")" << endl;
    #endif

    dataset.extend(dataset_dimension);
}

H5::Group H5FormatUtils::create_group(H5::Group& target, const string& name)
{
    return target.createGroup(name.c_str());
}

const boost::any& H5FormatUtils::get_value_from_reference(const string& dataset_name, const boost::any& value_reference, const map<string, boost::any>& values)
{
    try {
        auto reference_string = boost::any_cast<string>(value_reference);
        
        #ifdef DEBUG_OUTPUT
            cout << "[H5FormatUtils::get_value_from_reference] Getting dataset '"<< dataset_name;
            cout << "' reference value '" << reference_string << "'." << endl;
        #endif

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

const H5::PredType& H5FormatUtils::get_dataset_data_type(const string& type)
{
    #ifdef DEBUG_OUTPUT
        cout << "[H5FormatUtils::get_dataset_data_type] Getting dataset type for received frame type " << type << endl;
    #endif

    if (type == "uint8") {
        return H5::PredType::NATIVE_UINT8;

    } else if (type == "uint16") {
        return H5::PredType::NATIVE_UINT16;

    } else if (type == "uint32") {
        return H5::PredType::NATIVE_UINT32;

    }if (type == "int8") {
        return H5::PredType::NATIVE_INT8;
        
    } else if (type == "int16") {
        return H5::PredType::NATIVE_INT16;

    } else if (type == "int32") {
        return H5::PredType::NATIVE_INT32;

    } else {
        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "Unsupported array data_type " << type << endl;

        throw runtime_error(error_message.str());
    }
}

H5::DataSet H5FormatUtils::write_dataset(H5::Group& target, const h5_dataset& dataset, const map<string, boost::any>& values)
{
    const string& name = dataset.name;
    boost::any value;

    // Value is stored directly in the struct.
    if (dataset.data_location == IMMEDIATE){
        value = dataset.value;
    // Value in struct is just a string reference to into the values map.
    } else {
        value = H5FormatUtils::get_value_from_reference(name, dataset.value, values);
    }
    
    if (dataset.data_type == NX_CHAR || dataset.data_type == NX_DATE_TIME || dataset.data_type == NXnote) {
        // Attempt to convert to const char * (string "literals" cause that).
        try {
            return H5FormatUtils::write_dataset(target, name, string(boost::any_cast<const char*>(value)));
        } catch (const boost::bad_any_cast& exception) {}

        // Atempt to convert to string.
        try {
            return H5FormatUtils::write_dataset(target, name, boost::any_cast<string>(value));
        } catch (const boost::bad_any_cast& exception) {}

        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "Cannot convert dataset " << name << " to string or const char*." << endl;

        throw runtime_error(error_message.str());

    } else if (dataset.data_type == NX_INT) {
        try {
            return H5FormatUtils::write_dataset(target, name, boost::any_cast<int>(value));
        } catch (const boost::bad_any_cast& exception) {}

        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "Cannot convert dataset " << name << " to NX_INT." << endl;

        throw runtime_error(error_message.str());
    } else if (dataset.data_type == NX_FLOAT || dataset.data_type == NX_NUMBER) {
        try {
            return H5FormatUtils::write_dataset(target, name, boost::any_cast<double>(value));
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

H5::DataSet H5FormatUtils::write_dataset(H5::Group& target, const string& name, double value)
{
    H5::DataSpace att_space(H5S_SCALAR);
    auto data_type = H5::PredType::NATIVE_DOUBLE;

    H5::DataSet dataset = target.createDataSet(name.c_str(), data_type , att_space);
    dataset.write(&value, data_type);

    return dataset;
}

H5::DataSet H5FormatUtils::write_dataset(H5::Group& target, const string& name, int value)
{
    H5::DataSpace att_space(H5S_SCALAR);
    auto data_type = H5::PredType::NATIVE_INT;

    H5::DataSet dataset = target.createDataSet(name.c_str(), data_type, att_space);
    dataset.write(&value, data_type);

    return dataset;
}

H5::DataSet H5FormatUtils::write_dataset(H5::Group& target, const string& name, const string& value)
{
    H5::DataSpace att_space(H5S_SCALAR);
    H5::DataType data_type = H5::StrType(0, H5T_VARIABLE);

    H5::DataSet dataset = target.createDataSet(name.c_str(), data_type ,att_space);
    dataset.write(&value, data_type);

    return dataset;
}

void H5FormatUtils::write_attribute(H5::H5Object& target, const string& name, const string& value)
{
    H5::DataSpace att_space(H5S_SCALAR);
    H5::DataType data_type = H5::StrType(H5::PredType::C_S1, H5T_VARIABLE);

    auto h5_attribute = target.createAttribute(name.c_str(), data_type, att_space);
    h5_attribute.write(data_type, &value);
}

void H5FormatUtils::write_attribute(H5::H5Object& target, const string& name, int value)
{
    H5::DataSpace att_space(H5S_SCALAR);
    auto data_type = H5::PredType::NATIVE_INT;

    auto h5_attribute = target.createAttribute(name.c_str(), data_type, att_space);
    h5_attribute.write(data_type, &value);
}

void H5FormatUtils::write_attribute(H5::H5Object& target, const h5_attr& attribute, const map<string, boost::any>& values) 
{
    string name = attribute.name;
    boost::any value;

    // Value is stored directly in the struct.
    if (attribute.data_location == IMMEDIATE){
        value = attribute.value;
    // Value in struct is just a string reference to into the values map.
    } else {
        value = H5FormatUtils::get_value_from_reference(name, attribute.value, values);
    }
    
    if (attribute.data_type == NX_CHAR) {
        // Attempt to convert to const char * (string "literals" cause that).
        try {
            H5FormatUtils::write_attribute(target, name, string(boost::any_cast<const char*>(value)));
            return;
        } catch (const boost::bad_any_cast& exception) {}

        // Atempt to convert to string.
        try {
            H5FormatUtils::write_attribute(target, name, boost::any_cast<string>(value));
            return;
        } catch (const boost::bad_any_cast& exception) {}

        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "Cannot convert attribute " << name << " to string or const char*." << endl;

        throw runtime_error(error_message.str());

    } else if (attribute.data_type == NX_INT) {
        try {
            H5FormatUtils::write_attribute(target, name, boost::any_cast<int>(value));
            return;
        } catch (const boost::bad_any_cast& exception) {}

        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "Cannot convert attribute " << name << " to INT." << endl;

        throw runtime_error(error_message.str());
    }
}

void H5FormatUtils::write_format_data(H5::Group& file_node, const h5_parent& format_node, const std::map<std::string, h5_value>& values) 
{
    auto node_group = H5FormatUtils::create_group(file_node, format_node.name);

    for (const auto item_ptr : format_node.items) {
        const h5_base& item = *item_ptr;

        if (item.node_type == GROUP) {
            auto sub_group = dynamic_cast<const h5_group&>(item); 

            write_format_data(node_group, sub_group, values);

        } else if (item.node_type == ATTRIBUTE) {
            auto sub_attribute = dynamic_cast<const h5_attr&>(item);
            
            H5FormatUtils::write_attribute(node_group, sub_attribute, values);

        } else if (item.node_type == DATASET) {
            auto sub_dataset = dynamic_cast<const h5_dataset&>(item);
            auto current_dataset = H5FormatUtils::write_dataset(node_group, sub_dataset, values);

            for (const auto dataset_attr_ptr : sub_dataset.items) {
                const h5_base& dataset_attr = *dataset_attr_ptr;

                // You can specify only attributes inside a dataset.
                if (dataset_attr.node_type != ATTRIBUTE) {
                    stringstream error_message;
                    error_message << "Invalid element " << dataset_attr.name << " on dataset " << sub_dataset.name << ". Only attributes allowd.";

                    throw invalid_argument( error_message.str() );
                }

                auto sub_attribute = dynamic_cast<const h5_attr&>(dataset_attr);

                H5FormatUtils::write_attribute(current_dataset, sub_attribute, values);
            }
        }
    }
}

void H5FormatUtils::write_format(H5::H5File& file, const H5Format& format, const std::map<std::string, h5_value>& input_values)
{
    auto format_definition = format.get_format_definition();
    auto default_values = format.get_default_values();

    map<string, boost::any> format_values(default_values);
    
    format.add_input_values(format_values, input_values);
    format.add_calculated_values(format_values);
    
    write_format_data(file, format_definition, format_values);

    file.move(format.get_raw_frames_dataset_name().c_str(), format.get_frames_dataset_name().c_str());
}