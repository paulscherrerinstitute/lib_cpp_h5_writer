#include <iostream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <memory>

#include "../config.hpp"
#include "../H5Format.hpp"

using namespace std;
using s_ptr = shared_ptr<h5_base>;

class SfFormat : public H5Format
{
    shared_ptr<unordered_map<string, DATA_TYPE>> input_value_type = NULL;
    shared_ptr<unordered_map<string, boost::any>> default_values = NULL;
    shared_ptr<unordered_map<string, std::string>> dataset_move_mapping = NULL;
    shared_ptr<h5_parent> file_format = NULL;

    public:
        ~SfFormat(){};

        SfFormat()
        {
            // Input values definition type. 
            // Which type should be the parameters you receive over the REST api.
            input_value_type.reset(
            new unordered_map<string, DATA_TYPE>({
                {"file_info/date", NX_CHAR},
                {"file_info/version", NX_CHAR},
                {"file_info/owner", NX_CHAR},
                {"file_info/instrument", NX_CHAR},
                {"experiment_info/Pgroup", NX_CHAR},
            }));

            // Default values used in the file format.
            default_values.reset(new std::unordered_map<string, boost::any>({}));    

            // After format has been writen, where to move the raw datasets.
            dataset_move_mapping.reset(new std::unordered_map<string, string>(
            {
                {config::raw_image_dataset_name, "data/image"},
                {"pulse_id", "data/pulse_id"},
            }));

            // Definition of the file format.
            file_format.reset(
            new h5_parent("", EMPTY_ROOT, {
                s_ptr(new h5_group("file_info", {
                    s_ptr(new h5_dataset("Date", "file_info/date", NX_DATE_TIME)),
                    s_ptr(new h5_dataset("Version", "file_info/version", NX_CHAR)),
                    s_ptr(new h5_dataset("Owner", "file_info/owner", NX_CHAR)),
                    s_ptr(new h5_dataset("Instrument", "file_info/instrument", NX_CHAR)),
                })),

                s_ptr(new h5_group("experiment_info", {
                    s_ptr(new h5_dataset("Pgroup", "experiment_info/Pgroup", NX_CHAR)),
                })),

                s_ptr(new h5_group("data")),
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