#ifndef H5_FILE_FORMAT_H
#define H5_FILE_FORMAT_H

#include <string>
#include <list>

enum DATA_TYPE {
    NX_FLOAT,
    NX_CHAR,
    NX_INT,
    NX_DATE_TIME,
    NX_NUMBER,
    NXnote
};

struct h5_base {
    h5_base(std::string name) : name(name), value_alias(name){};
    h5_base(std::string name, std::string value_alias) : name(name), value_alias(value_alias){};
    std::string name;
    std::string value_alias;
};

struct h5_parent{
    h5_parent(std::list<h5_base> items) : items(items) {};
    std::list<h5_base> items;
};

struct h5_group : public h5_base, public h5_parent {
    h5_group(std::string name, std::list<h5_base> items) : h5_base(name), h5_parent(items) {};
};

struct h5_dataset : public h5_base, public h5_parent{
    h5_dataset(std::string name, DATA_TYPE type, std::list<h5_base> items={}) : h5_base(name), h5_parent(items), type(type) {};
    h5_dataset(std::string name, std::string value_alias, DATA_TYPE type, std::list<h5_base> items={}) : h5_base(name, value_alias), h5_parent(items), type(type) {};
    DATA_TYPE type;
};

struct h5_attribute : public h5_base {
    h5_attribute(std::string name): h5_base(name){};
    h5_attribute(std::string name, std::string value_alias): h5_base(name, value_alias){};
};

struct h5_attribute_val : public h5_attribute {
    h5_attribute_val(std::string name, std::string value): h5_attribute(name){};
    std::string value;
};

#endif