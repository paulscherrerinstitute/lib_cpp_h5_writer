# libCppH5Writer
This is the library used for creating C++ based stream writer for H5 files.

It focuses on the functionality needed for high performance detectors integrations.

Key features:
- Get data from ZMQ stream (Array-1.0 protocol) - [htypes specification](https://github.com/datastreaming/htypes)
- Write Nexus complaint H5 file (User specified) - [nexus format] (http://www.nexusformat.org/)
- Specify additional zmq stream parameters to write to file.
- Receive additional parameters to write to file via REST api.
- Interaction with the writer over REST api (stop, kill, get_statistics).

# Table of content
1. [Quick start using the library](#quick_start)
2. [Build](#build)
    1. [Conda setup](#conda_setup)
    2. [Local build](#local_build)
3. [Basic concepts](#basic_concepts)
    1. [ZmqReceiver](#zmq_receiver)
        [Stream header values](#stream_header_values)
    2. [H5Writer](#h5_writer)
    3. [H5Format](#h5_format)
4. [REST interface](#rest_interface)
5. [Examples](#examples)


<a id="quick_start"></a>
# Quick start using the library

To create your own stream writer you need to specify:
- The H5 file format you want to write.
- The mapping of REST input variables to your H5 format.
- Additional H5 format fields with default values or calculated fields (based on input or default values).
- The mapping between the stream header metadata and your H5 file format.
- Additional metadata that is transfer in the stream message header.


For example, lets see the SF file format. You will need to extend the abstract class H5Format. Lets 
save this file into **SfFormat.cpp**.
```cpp
#include <memory>
#include <unordered_map>

#include "config.hpp"
#include "H5Format.hpp"

using namespace std;
using s_ptr = shared_ptr<h5_base>;

class SfFormat: public H5Format
{
    shared_ptr<unordered_map<string, DATA_TYPE>> input_value_type = NULL;
    shared_ptr<unordered_map<string, boost::any>> default_values = NULL;
    shared_ptr<unordered_map<string, std::string>> dataset_move_mapping = NULL;
    shared_ptr<h5_parent> file_format = NULL;

    public:
        SfFormat(){

            // Defines the input variables the user can set via the REST api.
            // {"variable_name", [NEXUS_VARIABLE_TYPE]}
            input_value_type.reset(
            new unordered_map<string, DATA_TYPE>({
                {"file_info/date", NX_CHAR},
                {"file_info/owner", NX_CHAR},
                {"file_info/instrument", NX_CHAR},
                {"experiment_info/Pgroup", NX_CHAR},
            }));

            // Specify the values of default parameters, for example the file format.
            // {"variable_name", [value]}
            default_values.reset(new std::unordered_map<string, boost::any>({
                {"file_info/version", "1.0.0"},
            }));

            // Specify how to map datasets the H5Writer produces and 
            // incorporate them into your file format. See H5Format chapter for more info.
            // {"source_dataset_path", "destination_dataset_path"}
            dataset_move_mapping.reset(new std::unordered_map<string, string>(
            {
                {config::raw_image_dataset_name, "data/image"},
                {"pulse_id", "data/pulse_id"},
            }));

            // The H5 file format specification.
            // See H5Format chapter for more info.
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

        // Just return the format definition. Needed when writing the format.
        const h5_parent& get_format_definition() const override 
        {
            return *file_format;
        }

        // Just return the default values. Needed when writing the format.
        const unordered_map<string, boost::any>& get_default_values() const override 
        {
            return *default_values;
        }

        // Add any calculated fields to the values map. 
        // The input values are in the values map as well.
        void add_calculated_values(unordered_map<string, boost::any>& values) const override 
        {
            // No calculated values.
        }

        // This functions adds REST parameters to your H5 variables - the input mapping might not be 1:1.
        // If you need to rename of modify your input variables, do it here.
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

        // Just return the input value types - neede by the REST interface.
        const std::unordered_map<string, DATA_TYPE>& get_input_value_type() const override 
        {
            return *input_value_type;
        }

        const unordered_map<string, string>& get_dataset_move_mapping() const override {
            return *dataset_move_mapping;
        }
};
```

In addition, you need to write a starter script for your writer. Lets again see the SF example.
This file is called **sf\_h5\_zmq\_writer\_h5**.
```cpp
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "config.hpp"
#include "ProcessManager.hpp"
#include "WriterManager.hpp"
#include "ZmqReceiver.hpp"
#include "SfFormat.cpp"

int main (int argc, char *argv[])
{
    if (argc != 6) {
        cout << endl;
        cout << "Usage: sf_h5_zmq_writer [connection_address] [output_file] [n_frames] [rest_port] [user_id]" << endl;
        cout << "\tconnection_address: Address to connect to the stream (PULL). Example: tcp://127.0.0.1:40000" << endl;
        cout << "\toutput_file: Name of the output file." << endl;
        cout << "\tn_frames: Number of images to acquire. 0 for infinity (untill /stop is called)." << endl;
        cout << "\trest_port: Port to start the REST Api on." << endl;
        cout << "\tuser_id: uid under which to run the writer. -1 to leave it as it is." << endl;
        cout << endl;

        exit(-1);
    }

    // This process can be set to run under a different user.
    auto user_id = atoi(argv[5]);
    if (user_id != -1) {

        #ifdef DEBUG_OUTPUT
            cout << "[sf_h5_zmq_writer::main] Setting process uid to " << user_id << endl;
        #endif

        if (setuid(user_id)) {
            stringstream error_message;
            error_message << "[sf_h5_zmq_writer::main] Cannot set user_id to " << user_id << endl;

            throw runtime_error(error_message.str());
        }
    }

    int n_frames =  atoi(argv[3]);
    string output_file = string(argv[2]);

    // Instantiate the file format you defined above.
    SfFormat format;

    // Create a writer manager with the input value types. This will be passed to the REST api.
    WriterManager manager(format.get_input_value_type(), output_file, n_frames);

    string connect_address = string(argv[1]);
    int n_io_threads = config::zmq_n_io_threads;
    int receive_timeout = config::zmq_receive_timeout;
    
    // Define additional stream header fields to be written in the H5 file - pulse_id, in this case.
    // For more info consult the ZmqReceiver chapter.
    // {"field_name", "protocol value type"}
    auto header_values = shared_ptr<unordered_map<string, string>>(new unordered_map<string, string> {
        {"pulse_id", "uint64"},
    });
    ZmqReceiver receiver(connect_address, n_io_threads, receive_timeout, header_values);

    int rest_port = atoi(argv[4]);

    // Start the writer. This is a blocking call.
    ProcessManager::run_writer(manager, format, receiver, rest_port);

    return 0;
}

```

<a id="build"></a>
# Build

The easiest way to build the library is via Anaconda. If you are not familiar with Anaconda (and do not want to learn), 
please see the [Local build](#local_build) chapter.

<a id="conda_setup"></a>
## Conda setup
If you use conda, you can create an environment with the libCppH5Writer library by running:

```bash
conda create -c paulscherrerinstitute --name <env_name> libCppH5Writer
```

After that you can just source you newly created environment and start linking your builds against the library.

<a id="local_build"></a>
## Local build
You can build the library by running make in the root folder of the project:

```bash
make clean all
```

or by using the conda also from the root folder of the project:

```bash
conda build conda-recipe
```

Both methods require you to have a sourced conda environament with the below specified requirements installed.

### Requirements
The library relies on the following packages:

- make
- gcc
- cppzmq ==4.2.1
- hdf5 ==1.10.1
- boost ==1.61.0

When you are using conda to install the packages, you might need to add the **conda-forge** channel to
your conda config:

```
conda config --add channels conda-forge
```

<a id="basic_concepts"></a>
# Basic concepts
In this chapter we will describe the basic concepts you need to get a hold off in order to use the library.
In case more advanced knowledge is needed, please feel free to browse the code. The most important components 
are discussed in subchapters below.

**General overview**

The writer has 3 processes:
- REST process (listens to incoming REST requests).
- ZMQ process (listens for incoming ZMQ stream messages).
- H5 process (writes the received data to disk).

The communication bridges between processes are:
- REST to H5 process: WriterManager (WriterManager.cpp).
- ZMQ to H5 process: WriterManager for process control (WriterManager.cpp) and RingBuffer (RingBuffer.cpp) for data transfer.

In order to have a central place where to set fine tunning parameters, the **config.cpp** file is used.

The ZMQ process receives data from the stream, it extracts it and packs it (with additional metadata) into the ring buffer. 
Meanwhile, the H5 process is listening for data in the ring buffer. When new data arrives, it writes this data down into 
temporary datasets (for performance reasons we write the file format in the end).

When the end of the writing is triggered (via the REST api, when the desired number of frames are received, or when the user 
terminates the process), an attempt to write the file format is performed. If the format writing is successful, the temporary 
datasets are moved to their final place in the file format. If the format writing step fails for any reason, the data will 
remain in the temporary datasets and the user will need to fix the file manually (the goal is to preserve the data as much as possible).

<a id="zmq_receiver"></a>
## ZmqReceiver
The stream receiver that gets your data from the stream. This is PSI specific, and currently supports only the **Array-1.0** protocol.

The protocol specification can be found here: [htypes specification](https://github.com/datastreaming/htypes)

<a id="stream_header_values"></a>
### Stream header values

In addition to the image in the stream, the receiver can pass to the writer also data defined in the header of the stream, for example:
- pulse_id (The pulse id for the current image)
- source (source of the currect image)
- etc.

This fields are specific to your input stream. The only constrain is that values should be scalars (one value per message). 
The allowed data types for this values are:

- "uint8"
- "uint16"
- "uint32"
- "uint64"
- "int8"
- "int16"
- "int32"
- "int64"
- "float32"
- "float64"

This stream header parameters need to be specified when constructing your ZmqReceiver instance:
```cpp
// Extract the "pulse_id" value from the header, and convert it into uint64 type.
auto header_values = shared_ptr<unordered_map<string, string>>(new unordered_map<string, string> {
    {"pulse_id", "uint64"},
});

// Pass the header_values to the ZmqReceiver constructor.
ZmqReceiver receiver(connect_address, n_io_threads, receive_timeout, header_values);
```

Read the [H5Writer](#h5_writer) chapter to see where this data is written in the H5 file. 
Knowing where the data is written is important to properly setup the **dataset\_move\_mapping** 
in the file format. See chapter [H5Format](#h5_format) for more info.

<a id="h5_writer"></a>
## H5Writer

<a id="h5_format"></a>
## H5Format

<a id="rest_interface"></a>
# REST interface

<a id="examples"></a>
# Examples