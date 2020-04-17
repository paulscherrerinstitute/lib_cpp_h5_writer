#include "BinaryWriter.hpp"
#include <unistd.h>
#include <iostream>
#include "date.h"
#include <cerrno>
#include <chrono>
#include <cstring>
#include <buffer_utils.hpp>

using namespace std;

BinaryWriter::BinaryWriter(
        const string& device_name,
        const string& root_folder) :
        device_name_(device_name),
        root_folder_(root_folder),
        current_output_filename_(""),
        output_file_fd_(-1)
{
    latest_filename_ = root_folder + "/" + device_name + "/LATEST";

    #ifdef DEBUG_OUTPUT
        using namespace date;
        using namespace chrono;
        cout << "[" << system_clock::now() << "]";
        cout << "[BinaryWriter::BinaryWriter]";
        cout << " Starting with";
        cout << " device_name " << device_name_;
        cout << " root_folder_ " << root_folder_;
        cout << " latest_filename_ " << latest_filename_;
        cout << endl;
    #endif
}

void BinaryWriter::write(uint64_t pulse_id, const JFFileFormat* buffer)
{
    auto current_frame_file =
            get_filename(root_folder_, device_name_, pulse_id);

    if (current_frame_file != current_output_filename_) {
        close_current_file();
        open_file(current_frame_file);
    }

    auto n_bytes = ::write(output_file_fd_, buffer, sizeof(JFFileFormat));
    if (n_bytes < sizeof(JFFileFormat)) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono;
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[BinaryWriter::write";
        err_msg << " Error while writing to file ";
        err_msg << current_output_filename_ << ": ";
        err_msg << strerror(errno) << endl;

        throw runtime_error(err_msg.str());
    }

    auto sync_result = ::fdatasync(output_file_fd_);
    if (sync_result < 0) {
        stringstream err_msg;

        using namespace date;
        using namespace chrono;
        err_msg << "[" << system_clock::now() << "]";
        err_msg << "[BinaryWriter::write";
        err_msg << " Error while fdatasync on file ";
        err_msg << current_output_filename_ << ": ";
        err_msg << strerror(errno) << endl;

        throw runtime_error(err_msg.str());
    }
}

void BinaryWriter::close_current_file()
{
    if (output_file_fd_ != -1) {
        if (close(output_file_fd_) < 0) {
            stringstream err_msg;

            using namespace date;
            using namespace chrono;
            err_msg << "[" << system_clock::now() << "]";
            err_msg << "[BinaryWriter::close_current_file]";
            err_msg << " Error while closing file ";
            err_msg << current_output_filename_ << ": ";
            err_msg << strerror(errno) << endl;

            throw runtime_error(err_msg.str());
        }

        output_file_fd_ = -1;

        // TODO: Ugly hack, please please fix it.
        stringstream latest_command;
        latest_command << "echo " << current_output_filename_;
        latest_command << " > " << latest_filename_;
        auto str_latest_command = latest_command.str();

        system(str_latest_command.c_str());
    }

    current_output_filename_ = "";
}