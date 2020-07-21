[![Build Status](https://api.travis-ci.org/paulscherrerinstitute/lib_cpp_h5_writer.svg?branch=master)](https://travis-ci.org/paulscherrerinstitute/lib_cpp_h5_writer/) 

# lib_cpp_h5_writer
This library is used for creating C++ based stream writer for H5 files. It focuses on the functionality 
and performance needed for high performance detectors integrations.

Key features:
- Get data from ZMQ stream (Array-1.0 protocol) - [htypes specification](https://github.com/datastreaming/htypes)
- Write Nexus complaint H5 file (User specified) - [nexus format](http://www.nexusformat.org/)
- Specify additional zmq stream parameters to write to file.
- Receive additional parameters to write to file via REST api.
- Interaction with the writer over REST api (stop, kill, get_statistics).

# Table of content
1. [Quick start using the library](#quick_start)
2. [Build](#build)
    1. [Conda build](#conda_build)
    2. [Local build](#local_build)
3. [Basic concepts](#basic_concepts)
    1. [ProcessManager](#process_manager)
    2. [ZmqReceiver](#zmq_receiver)
    3. [H5Writer](#h5_writer)
    4. [H5Format](#h5_format)
    5. [WriterManager](#writer_manager)
    5. [RingBuffer](#ring_buffer)
4. [REST interface](#rest_interface)
5. [Examples](#examples)


<a id="quick_start"></a>
# Quick start for using the library

To create your own stream writer you need to specify:
- The H5 file format you want to write.
- The mapping of REST input variables to your H5 format.
- Additional H5 format fields with default values or calculated fields (based on input or default values).
- The mapping between the stream header metadata and your H5 file format.
- Additional metadata that is transfer in the stream message header.

Under **sf/** and **csaxs/** you can see examples of this. Feel free to use any of this folders as a template.

**IMPORTANT**: We are using a monorepo for this project (all implementations should live in this git repository).
To create a new implementation, please add a folder to the root of the proejct (like sf/ and csaxs/).

The minimum you need to implement your own writer is:
- Writer runner (example: csaxs/csaxs\_h5\_writer.cpp)
- File format (example: csaxs/CsaxsFormat.cpp)
- Build file (example: csaxs/Makefile)

## Writer runner
Example: **csaxs/csaxs\_h5\_writer.cpp**

The runner is the actual executable you will run to create files. In the writer runner you:
- Specify and parse input parameters.
- Prepare your system for writing (creating folders, switch process user etc.)
- Instantiate the file format object.
- Define the parameters that come in the stream header.
- Start the writer (mostly boilerplate code, if you do not need any special implementations).

## File format
Example: **csaxs/CsaxsFormat.cpp**

This is a class that extends the **H5Format** class. You need to specify:
- input\_value\_type (REST API value name to type mapping)
- default\_values (Fields in the file format that have default values)
- dataset\_move\_mapping (Move datasets to another place in the file if needed)
- file_format (The hierarchical structure of your H5 format)
It is best to specify all the values above in the class constructor. Some values (all except file_format) can be empty, 
but they should not be null.

The current cSAXS and SF formats are quite simple. As a reference, you can check the old cSAXS file format implementation: 
[csaxs_cpp_h5_writer](https://github.com/paulscherrerinstitute/csaxs_cpp_h5_writer/blob/master/CsaxsFormat.cpp)

## Build file
Example: **csaxs/Makefile**

If you want to use Makefiles, you can basically copy one from an existing implementation (csaxs/) and rename the executable. In 
case you want something more sophisticated you will have to provide it yourself.

In addition, you can deploy your writer also as an anaconda package - you will need to include the conda-recipe folder in this case 
as well (see csaxs/conda-recipe).

<a id="build"></a>
# Build

**You need your compiler to support C++11.**

The easiest way to build the library is via Anaconda. If you are not familiar with Anaconda (and do not want to learn), 
you can also install all the dependencies directly in your os.

The base library is located in **lib/**. Change you current directory to lib/ and:
- make (build the library for production)
- make clean (clean the previous build)
- make deploy (deploy library to your local conda environemnt)
- make debug (build library with debug prints in the standard output)
- make perf (build the library with performance measurements in the standard output)
- make test (create tests)

The usual procedure would be:
- make test (build the tests)
- ./bin/execute_tests (execute the tests)
- make deploy (deploy the library)

You can then start building your executable. It is also a good idea to automate the base library build from your executable build system 
(see csaxs/Makefile, lib target for example).

<a id="conda_build"></a>
## Conda build
If you use conda, you can create an environment with the needed library by running:

```bash
conda create -c paulscherrerinstitute --name <env_name> make cppzmq==4.3.0 hdf5==1.10.4 boost==1.61.0 gtest==1.8.1
```

After that you can just source you newly created environment:

```base
conda activate <env_name> 
```

and start linking your builds against the libraries. To do that you can use the environament variables Anaconda sets:

```bash
-L${CONDA_PREFIX}/lib (for linking libraries you have installed with Anaconda)
```

To run you executables inside the Anaconda environment, you will need also to export the lib/ path in your env variables:
```bash
export LD_LIBRARY_PATH=${CONDA_PREFIX}/lib
```

<a id="local_build"></a>
## Local build

If you decide not to use Anaconda, you will have to install the following libraries in your system:

- make
- cppzmq ==4.3.0
- hdf5 ==1.10.4
- boost ==1.61.0

<a id="basic_concepts"></a>
# Basic concepts
In this chapter we will describe the basic concepts you need to get a hold off in order to use the library.
In case more advanced knowledge is needed, please feel free to browse the code. The most important components 
are discussed in subchapters below.

**General overview**

The process and thread management is taken care by the [ProcessManager](#process_manager). The process manager initializes, 
starts and stops the 3 threads discussed below.

The writer has 3 threads:
- ZMQ receiving thread (listens for incoming ZMQ stream messages).
    - [ZmqReceiver](#zmq_receiver) is the only class really used here.
- H5 writer thread (writes the received data to disk).
    - [H5Writer](#h5_writer) is the base writer implementation that can be extended at will.
- REST thread (listens to incoming REST requests).
    - [REST interface](#rest_interface) describes how the REST interface works. 

The communication bridges between threads are:
- REST to H5 thread: [WriterManager](#writer_manager).
- ZMQ to H5 thread: [WriterManager](#writer_manager) for process control and [RingBuffer](#ring_buffer) for data transfer.

In order to have a central place where to set fine tunning parameters, the **config.cpp** file is used.

The ZMQ thread receives data from the stream, it extracts it and packs it (with additional metadata) into the ring buffer. 
Meanwhile, the H5 thread is listening for data in the ring buffer. When new data arrives, it writes this data down into 
temporary datasets (for performance reasons we write the file format in the end).

When the end of the writing is triggered (via the REST api, when the desired number of frames are received, or when the user 
terminates the process), an attempt to write the file format is performed. If the format writing is successful, the temporary 
datasets are moved to their final place in the file format. If the format writing step fails for any reason, the data will 
remain in the temporary datasets and the user will need to fix the file manually (the goal is to preserve the data as much as possible).

<a id="process_manager"></a>
## ProcessManager

Not yet here :(

<a id="zmq_receiver"></a>
## ZmqReceiver
The stream receiver that gets your data from the stream. This is PSI specific, and currently supports only the **Array-1.0** protocol.
You pass the ZmqReceiver you would like to use in your writer runner, so it should be easy to implement your own if needed.

The protocol specification can be found here: [htypes specification](https://github.com/datastreaming/htypes)

<a id="stream_header_values"></a>
### Stream header values

In addition to the image in the stream, the receiver can pass to the writer also data defined in the header of the stream, for example:
- pulse_id (The pulse id for the current image)
- source (source of the currect image)
- etc.

This fields are specific to your input stream, and you specify them in your writer runner. You can define both scalars and arrays 
(see **csaxs/sf\_h5\_writer.cpp**, variable **header\_values** for an example).

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
auto header_values = shared_ptr<unordered_map<string, HeaderDataType>>(new unordered_map<string, HeaderDataType> {
    {"frame", HeaderDataType("uint64")}, // Scalar for frame number
    {"module_number", HeaderDataType("int64", n_modules)} // Array of n_modules elements for module_number.
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

The H5Format is the base class you need to extend to implement your file format. It specifies that the following variables need to be set:
- input\_value\_type (REST API value name to type mapping)
- default\_values (Fields in the file format that have default values)
- dataset\_move\_mapping (Move datasets to another place in the file if needed)
- file_format (The hierarchical structure of your H5 format)

We will discuss each one in details in this chapter.

### input\_value\_type

Not yet here :(

### default\_values

Not yet here :(

### dataset\_move\_mapping

According to the [Scientific Exchange Data format](https://pubmed.ncbi.nlm.nih.gov/25343788/), the structure of the dataset inside the hdf5 file will be:

config::raw_image_dataset_name : "exchange/" + dataset_name
htype : "measurement/acquisition/"+dataset_name+"/htype"
tag : "measurement/acquisition/"+dataset_name+"/tag"
source : "measurement/acquisition/"+dataset_name+"/source"
shape : "measurement/acquisition/"+dataset_name+"/shape"
frame : "measurement/acquisition/"+dataset_name+"/frame"
type : "measurement/acquisition/"+dataset_name+"/type"
endianess : "measurement/acquisition/"+dataset_name+"/endianess"

⋅⋅⋅ This is defined on TomcatFormat.cpp.⋅⋅


### file\_format

Not yet here :(

<a id="writer_manager"></a>
## WriterManager

Not yet here :(

<a id="RingBuffer"></a>
## RingBuffer

Not yet here :(

<a id="rest_interface"></a>
# REST interface

The REST interface will start and be available while the writer is running on the port given as parameter when initializing the writer. The table below shows the currently available endpoints:

| Endpoint | Description |
| --- | --- |
| `kill` | Kills the writer app. |
| `stop` | Stops the writer manager and exists. |
| `status` | Gets the status from the writer manager.  |
| `statistics` | Gets the statistics from the writer manager.  |
| `parameters` | [GET/POST] Allows to get and post parameters from the writer manager. |

The rest interface can be used directly using cURL command or with the client developed specifically to control it [pco_rclient](https://github.com/paulscherrerinstitute/pco_rclient).

### Get writer status using curl command:
curl -X GET http://<address:port>/<endpoint>

### PCO_RCLIENT

```
    usage: pco_rclient [-h] {start,stop,kill,status} ...

    Rest api pco writer

    optional arguments:
      -h, --help            show this help message and exit

    command:
      valid commands

      {start,stop,kill,status}
                            commands
        start               start the writer
        stop                stop the writer
        kill                kill the writer
        status              Retrieve the status of the writer
```

<a id="examples"></a>
# Examples

The tomcat's version of the library usage example is displayed below:
```
    Usage: tomcat_h5_writer [connection_address] [output_file] [n_frames] [user_id]  [n_modules] [rest_api_port] [dataset_name] [max_frames_per_file] [statistics_monitor_address] 

    ./tomcat_h5_writer tcp://pc9808:9999 output.h5 0 -1 1 9555 data_white 20000 tcp://*:8088
```

| Parameter | Description |
| --- | --- |
| `connection_address` | Address to connect to the stream (PULL). Example: tcp://127.0.0.1:40000 |
| `output_file` | Name of the output file. |
| `n_frames` | Number of images to acquire. 0 for infinity (until /stop is called). |
| `user_id` | uid under which to run the writer. -1 to leave it as it is. |
| `n_modules` | Number of detector modules to be written. |
| `rest_port` | Port to start the REST Api on. |
| `dataset_name` | Definition of the dataset name. |
| `frames_per_file` | Maximum number of frames for each h5 file. |
| `statistics_monitor_address` | TCP address to send writer's statistics. |

