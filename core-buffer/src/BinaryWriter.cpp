#include "BinaryWriter.hpp"
#include <unistd.h>
#include <iostream>
#include "date.h"
#include <cerrno>
#include <chrono>
#include <cstring>

using namespace std;

BinaryWriter::BinaryWriter(
        const string& device_name,
        const string& root_folder) :
            device_name_(device_name),
            root_folder_(root_folder),
            current_filename_(""),
            output_fd_(-1)
{

}

void BinaryWriter::write(uint64_t pulse_id, const JFFileFormat& buffer)
{

}

void BinaryWriter::close_current_file()
{
    if (output_fd_ != -1) {
        if (close(output_fd_) < 0) {
            stringstream err_msg;

            using namespace date;
            using namespace chrono;
            err_msg << "[" << system_clock::now() << "]";
            err_msg << "[BinaryWriter::close_current_file]";
            err_msg << " Error while closing file ";
            err_msg << current_filename_ << ": ";
            err_msg << strerror(errno) << endl;

            throw runtime_error(err_msg.str());
        }
    }
}