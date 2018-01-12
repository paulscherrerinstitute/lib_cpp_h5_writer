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

void h5_utils::set_attributes(H5::H5Object& target, std::list<h5_attribute>& attributes) {
    for (auto& attribute : attributes) {

        string name = attribute.name;
        
        if (attribute.data_type == NX_CHAR) {
            auto attribute_ptr = reinterpret_cast<h5_attribute_val<NX_CHAR>*>(&attribute);
            h5_utils::write_attribute(target, name, attribute_ptr->value);

        } else if (attribute.data_type == NX_INT) {
            auto attribute_ptr = reinterpret_cast<h5_attribute_val<NX_INT>*>(&attribute);
            h5_utils::write_attribute(target, name, attribute_ptr->value);
        }
    }
}