#include <memory>
#include <unordered_map>
#include <iostream>

#include "config.hpp"
#include "H5Format.hpp"
#include "ZmqSender.hpp"

using namespace std;
using s_ptr = shared_ptr<h5_base>;

class TomcatFormat : public H5Format
{
    shared_ptr<unordered_map<string, DATA_TYPE>> input_value_type = NULL;
    shared_ptr<unordered_map<string, boost::any>> default_values = NULL;
    shared_ptr<unordered_map<string, std::string>> dataset_move_mapping = NULL;
    shared_ptr<h5_parent> file_format = NULL;

    public:
        ~TomcatFormat(){};

        TomcatFormat(const string& dataset_name)
        {
            input_value_type.reset(new unordered_map<string, DATA_TYPE>());
            default_values.reset(new unordered_map<string, boost::any>());

            std::string htype_str = "htype";
            std::string tag_str = "tag";
            std::string source_str ="source";
            std::string shape_str ="shape";
            std::string frame_str ="frame";
            std::string type_str ="type";
            std::string endianess_str ="endianess";

            // After format has been writen, where to move the raw datasets.
            dataset_move_mapping.reset(new std::unordered_map<string, string>(
            {
                {config::raw_image_dataset_name, "exchange/" + dataset_name},
                {htype_str, "measurement/acquisition/"+dataset_name+"/htype"},
                {tag_str, "measurement/acquisition/"+dataset_name+"/tag"},
                {source_str, "measurement/acquisition/"+dataset_name+"/source"},
                {shape_str, "measurement/acquisition/"+dataset_name+"/shape"},
                {frame_str, "measurement/acquisition/"+dataset_name+"/frame"},
                {type_str, "measurement/acquisition/"+dataset_name+"/type"},
                {endianess_str, "measurement/acquisition/"+dataset_name+"/endianess"},
            }));


            // Definition of the file format.
            file_format.reset(
            new h5_parent("", EMPTY_ROOT, {
                s_ptr(new h5_group("measurement", {
                    s_ptr(new h5_group("acquisition",{
                        s_ptr(new h5_group(dataset_name, {}))
                    }))
                })), 
                s_ptr(new h5_group("exchange", {}))
            }));

        }

        const h5_parent& get_format_definition() const override
        {
            return *file_format;
        }

        const unordered_map<string, boost::any>& get_default_values() const override
        {
            return *default_values;
        }

        void add_calculated_values(unordered_map<string, boost::any>& values) const override
        {
            // No calculated values.
        }

        void add_input_values(unordered_map<string, boost::any>& values,
            const unordered_map<string, boost::any>& input_values) const override
        {
            // Input value mapping is 1:1.
            for (const auto& input_value : input_values) {
                const auto& name = input_value.first;
                const auto& value = input_value.second;

                values[name] = value;
            }
        }

        const std::unordered_map<string, DATA_TYPE>& get_input_value_type() const override
        {
            return *input_value_type;
        }

        const unordered_map<string, string>& get_dataset_move_mapping() const override {
            return *dataset_move_mapping;
        }

};
