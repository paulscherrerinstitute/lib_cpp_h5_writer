#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "h5_utils.hpp"

using namespace std;

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
    
    if (dataset.data_type == NX_CHAR) {
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
        error_message << "Cannot convert dataset " << name << " to INT." << endl;

        throw runtime_error(error_message.str());
    } else if (dataset.data_type == NX_FLOAT) {
        try {
            return h5_utils::write_dataset(target, name, boost::any_cast<double>(value));
        } catch (const boost::bad_any_cast& exception) {}

        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "Cannot convert dataset " << name << " to INT." << endl;

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
            h5_utils::write_attribute(target, name, boost::any_cast<int32_t>(value));
            return;
        } catch (const boost::bad_any_cast& exception) {}

        // We cannot really convert this attribute.
        stringstream error_message;
        error_message << "Cannot convert attribute " << name << " to INT." << endl;

        throw runtime_error(error_message.str());
    }
}