#ifndef H5_FILE_FORMAT_H
#define H5_FILE_FORMAT_H

#include <string>
#include <list>
#include <boost/any.hpp>
#include <H5Cpp.h>
#include <map>

typedef boost::any h5_value;

enum NODE_TYPE {
    ATTRIBUTE,
    DATASET,
    GROUP
};

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
    h5_base(std::string name, NODE_TYPE node_type) : name(name), node_type(node_type){};
    virtual ~h5_base(){};
    std::string name;
    NODE_TYPE node_type;
};

struct h5_data_base{
    h5_data_base(DATA_TYPE data_type, DATA_LOCATION data_location) : data_type(data_type), data_location(data_location) {};
    DATA_TYPE data_type;
    DATA_LOCATION data_location;
};

struct h5_parent: public h5_base{
    h5_parent(std::string name, NODE_TYPE node_type, std::list<h5_base*> items) : h5_base(name, node_type), items(items) {};
    std::list<h5_base*> items;
};

struct h5_group : public h5_parent {
    h5_group(std::string name, std::list<h5_base*> items) : h5_parent(name, GROUP, items) {};
};

struct h5_dataset : public h5_parent, public h5_data_base{
    h5_dataset(std::string name, std::string value, DATA_TYPE data_type, std::list<h5_base*> items={})
        : h5_parent(name, DATASET, items), h5_data_base(data_type, REFERENCE), value(value) {};
    
    std::string value;
};

struct h5_attr : public h5_base, public h5_data_base {
    h5_attr(std::string name, h5_value value, DATA_TYPE data_types, DATA_LOCATION data_location=IMMEDIATE)
        : h5_base(name, ATTRIBUTE), h5_data_base(data_types, data_location), value(value){};
    h5_value value;
};

namespace h5_utils{
    H5::Group create_group(H5::CommonFG& target, std::string name);

    H5::DataSet write_dataset(H5::Group& target, h5_dataset& dataset, std::map<std::string, boost::any>& values);
    H5::DataSet write_dataset(H5::Group& target, std::string name, double value);
    H5::DataSet write_dataset(H5::Group& target, std::string name, int value);
    H5::DataSet write_dataset(H5::Group& target, std::string name, std::string value);

    void write_attribute(H5::H5Object& target, h5_attr& attribute, std::map<std::string, boost::any>& values);
    void write_attribute(H5::H5Object& target, std::string name, std::string value);
    void write_attribute(H5::H5Object& target, std::string name, int value);

    boost::any get_value_from_reference(std::string& dataset_name, boost::any value_reference, std::map<std::string, boost::any>& values);
}

#endif