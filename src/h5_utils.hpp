#ifndef H5_FILE_FORMAT_H
#define H5_FILE_FORMAT_H

#include <string>
#include <list>
#include <boost/any.hpp>
#include <H5Cpp.h>

typedef boost::any h5_value;

enum DATA_TYPE {
    NX_FLOAT,
    NX_CHAR,
    NX_INT,
    NX_DATE_TIME,
    NX_NUMBER,
    NXnote
};

enum DATA_LOCATION {
    IMMEDIATE,
    REFERENCE
};


struct h5_base {
    h5_base(std::string name) : name(name){};
    std::string name;
};

struct h5_data_base{
    h5_data_base(DATA_TYPE data_type, DATA_LOCATION data_location) : data_type(data_type), data_location(data_location) {};
    DATA_TYPE data_type;
    DATA_LOCATION data_location;
};

struct h5_parent{
    h5_parent(std::list<h5_base> items) : items(items) {};
    std::list<h5_base> items;
};

struct h5_group : public h5_base, public h5_parent {
    h5_group(std::string name, std::list<h5_base> items) : h5_base(name), h5_parent(items) {};
};

struct h5_dataset : public h5_base, public h5_parent, public h5_data_base{
    h5_dataset(std::string name, std::string value, DATA_TYPE data_type, std::list<h5_base> items={})
        : h5_base(name), h5_parent(items), h5_data_base(data_type, REFERENCE), value(value) {};
    
    std::string value;
};

struct h5_attr : public h5_base, public h5_data_base {
    h5_attr(std::string name, h5_value value, DATA_TYPE data_types, DATA_LOCATION data_location=IMMEDIATE)
        : h5_base(name), h5_data_base(data_types, data_location), value(value){};
    h5_value value;
};

namespace h5_utils{
    H5::Group create_group(H5::CommonFG& target, std::string name);

    H5::DataSet write_dataset(H5::Group& target, h5_dataset& dataset);

    H5::DataSet write_dataset(H5::Group& target, std::string name, double value);
    H5::DataSet write_dataset(H5::Group& target, std::string name, int value);
    H5::DataSet write_dataset(H5::Group& target, std::string name, std::string value);

    void write_attribute(H5::H5Object& target, h5_attr& attribute);
    void write_attribute(H5::H5Object& target, std::string name, std::string value);
    void write_attribute(H5::H5Object& target, std::string name, int value);

    
}

#endif