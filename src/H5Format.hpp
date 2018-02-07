#ifndef H5_FILE_FORMAT_H
#define H5_FILE_FORMAT_H

#include <string>
#include <list>
#include <map>
#include <H5Cpp.h>
#include <memory>
#include <boost/any.hpp>


typedef boost::any h5_value;

enum NODE_TYPE 
{
    ATTRIBUTE,
    DATASET,
    GROUP
};

enum DATA_TYPE 
{
    NX_FLOAT,
    NX_CHAR,
    NX_INT,
    NX_DATE_TIME,
    NX_NUMBER,
    NXnote
};

enum DATA_LOCATION 
{
    IMMEDIATE,
    REFERENCE
};


struct h5_base 
{
    h5_base(const std::string& name, NODE_TYPE node_type) : name(name), node_type(node_type){};
    virtual ~h5_base(){}
    std::string name;
    NODE_TYPE node_type;
};

struct h5_data_base
{
    h5_data_base(DATA_TYPE data_type, DATA_LOCATION data_location) : data_type(data_type), data_location(data_location) {};
    virtual ~h5_data_base(){}
    DATA_TYPE data_type;
    DATA_LOCATION data_location;
};

struct h5_parent: public h5_base
{
    h5_parent(const std::string& name, NODE_TYPE node_type, const std::list<std::shared_ptr<h5_base>>& items) : 
        h5_base(name, node_type), items(items) {};
    std::list<std::shared_ptr<h5_base>> items;
};

struct h5_group : public h5_parent 
{
    h5_group(const std::string& name, const std::list<std::shared_ptr<h5_base>>& items) : 
        h5_parent(name, GROUP, items) {};
};

struct h5_dataset : public h5_parent, public h5_data_base
{
    h5_dataset(const std::string& name, const std::string& value, DATA_TYPE data_type, const std::list<std::shared_ptr<h5_base>>& items={})
        : h5_parent(name, DATASET, items), h5_data_base(data_type, REFERENCE), value(value) {};
    
    std::string value;
};

struct h5_attr : public h5_base, public h5_data_base 
{
    h5_attr(const std::string& name, const h5_value& value, DATA_TYPE data_types, DATA_LOCATION data_location=IMMEDIATE)
        : h5_base(name, ATTRIBUTE), h5_data_base(data_types, data_location), value(value){};
    h5_value value;
};

class H5Format
{
    public:
        virtual ~H5Format(){};

        virtual const std::map<std::string, DATA_TYPE>& get_input_value_type() const = 0;
        virtual const std::map<std::string, boost::any>& get_default_values() const = 0;
        virtual const h5_group& get_format_definition() const = 0;
        virtual void add_calculated_values(std::map<std::string, boost::any>& values) const = 0;
        virtual void add_input_values(std::map<std::string, boost::any>& values, const std::map<std::string, boost::any>& input_values) const = 0;
        
        virtual std::string get_raw_frames_dataset_name() const = 0;
        virtual std::string get_frames_dataset_name() const = 0;
};

namespace H5FormatUtils 
{
    hsize_t expand_dataset(H5::DataSet& dataset, hsize_t frame_index, hsize_t dataset_increase_step);
    void compact_dataset(H5::DataSet& dataset, hsize_t max_frame_index);

    H5::Group create_group(H5::Group& target, const std::string& name);
    H5::PredType get_dataset_data_type(const std::string& type);

    H5::DataSet write_dataset(H5::Group& target, const h5_dataset& dataset, const std::map<std::string, boost::any>& values);
    H5::DataSet write_dataset(H5::Group& target, const std::string& name, double value);
    H5::DataSet write_dataset(H5::Group& target, const std::string& name, int value);
    H5::DataSet write_dataset(H5::Group& target, const std::string& name, const std::string& value);

    void write_attribute(H5::H5Object& target, const h5_attr& attribute, const std::map<std::string, boost::any>& values);
    void write_attribute(H5::H5Object& target, const std::string& name, const std::string& value);
    void write_attribute(H5::H5Object& target, const std::string& name, int value);

    const boost::any& get_value_from_reference(const std::string& dataset_name, const boost::any& value_reference, const std::map<std::string, boost::any>& values);

    void write_format_data(H5::Group& file_node, const h5_parent& format_node, const std::map<std::string, h5_value>& values);
    void write_format(H5::H5File& file, const H5Format& format, const std::map<std::string, h5_value>& input_values);
};

#endif