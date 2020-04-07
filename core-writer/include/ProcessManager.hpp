#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include "H5WriteModule.hpp"
#include "ZmqRecvModule.hpp"

class ProcessManager 
{
    H5WriteModule& write_module_;
    ZmqRecvModule& recv_module_;

    public:
        ProcessManager(H5WriteModule& write_module,
                       ZmqRecvModule& recv_module);

        void start_rest_api(const uint16_t rest_port);

        void start_writing(
                const std::string output_file,
                const int n_frames,
                const int user_id);
        void stop_writing();

        void start_receiving(
                const std::string connect_address,
                const int n_receiving_threads);
        void stop_receiving();

        std::string get_status();

        std::unordered_map<std::string, float> get_statistics();

};

#endif
