#ifndef H5_FILE_FORMAT_H
#define H5_FILE_FORMAT_H

#include <string>
#include <list>

#include <H5Cpp.h>

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
    h5_dataset(std::string name, std::string value_alias, DATA_TYPE data_type, std::list<h5_base> items={})
        : h5_base(name), h5_parent(items), h5_data_base(data_type, REFERENCE) {};
};

struct h5_attribute : public h5_base, public h5_data_base {
    h5_attribute(std::string name, DATA_TYPE data_type): h5_base(name), h5_data_base(data_type, IMMEDIATE){};
    h5_attribute(std::string name, std::string value_alias, DATA_TYPE data_types): h5_base(name), h5_data_base(data_types, REFERENCE){};
};

template<DATA_TYPE T> struct h5_attribute_val: public h5_base, public h5_data_base {};

template<> struct h5_attribute_val<NX_CHAR>: public h5_attribute {
    h5_attribute_val(std::string name, std::string value) : h5_attribute(name, NX_CHAR), value(value) {};
    std::string value;
};

template<> struct h5_attribute_val<NX_INT>: public h5_attribute {
    h5_attribute_val(std::string name, int value) : h5_attribute(name, NX_CHAR), value(value) {};
    int value;
};

namespace h5_utils{
    void set_attributes(H5::H5Object& target, std::list<h5_attribute>& attributes);
    void write_attribute(H5::H5Object& target, std::string name, std::string value);
    void write_attribute(H5::H5Object& target, std::string name, int value);
}

#endif