#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>

#include "h5_file_format.hpp"

using namespace std;

void h5_utils::write_attribute(H5::H5Object& target, string name, string value){
    H5::StrType str_type(0, H5T_VARIABLE);
    H5::DataSpace att_space(H5S_SCALAR);

    H5::Attribute h5_attribute = target.createAttribute(name, str_type, att_space);
    h5_attribute.write(str_type, value);
}

void h5_utils::write_attribute(H5::H5Object& target, string name, int32_t value){
    H5::IntType int_type(H5::PredType::NATIVE_INT32);
    H5::DataSpace att_space(H5S_SCALAR);

    H5::Attribute h5_attribute = target.createAttribute(name, int_type, att_space);
    h5_attribute.write(int_type, &value);
}

void h5_utils::write_attribute(H5::H5Object& target, h5_attr& attribute) {

    cout<< attribute.name << endl;

    string name = attribute.name;
    boost::any value;

    if (attribute.data_location == IMMEDIATE){
        value = attribute.value;
    } else {
        // TODO: Implement value.
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
        error_message << "Cannot convert attribute " << name << "to string or const char*." << endl;

        throw runtime_error(error_message.str());

    } else if (attribute.data_type == NX_INT) {
        h5_utils::write_attribute(target, name, boost::any_cast<int32_t>(value));
    }
}