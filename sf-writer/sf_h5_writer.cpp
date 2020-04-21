#include <iostream>
#include <stdexcept>
#include <ZmqReceiver.hpp>
#include <ZmqRecvModule.hpp>
#include <H5WriteModule.hpp>
#include <ProcessManager.hpp>
#include "mpi.h"

#include "config.hpp"
#include "SfFormat.cpp"

using namespace std;

int main (int argc, char *argv[])
{
    if (argc != 5) {
        cout << endl;
        cout << "Usage: sf_h5_writer [device_name]";
        cout << " [output_file] [start_pulse_id] [stop_pulse_id]";
        cout << endl;
        cout << "\tdevice_name: Name of detector to write.";
        cout << "\toutput_file: Complete path to the output file.";
        cout << "\tstart_pulse_id: Start pulse_id of retrieval." << endl;
        cout << "\tstop_pulse_id: Stop pulse_id of retrieval." << endl;
        cout << endl;

        exit(-1);
    }

    string device_name = string(argv[1]);
    string output_file = string(argv[2]);
    uint64_t start_pulse_id = (uint64_t) atoi(argv[3]);
    uint64_t stop_pulse_id = (uint64_t) atoi(argv[4]);

    if (MPI_Init ( &argc, &argv ) != MPI_SUCCESS) {
        throw runtime_error("Cannot MPI init.");
    }

    int rank_id;
    if(MPI_Comm_rank (MPI_COMM_WORLD, &rank_id) != MPI_SUCCESS) {
        throw runtime_error("Cannot get the MPI rank.");
    }




    if (MPI_Finalize() != MPI_SUCCESS) {
        throw runtime_error("Cannot finalize MPI.");
    };

    return 0;
}
