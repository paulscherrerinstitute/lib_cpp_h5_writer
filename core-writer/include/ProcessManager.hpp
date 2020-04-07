#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include "WriterUtils.hpp"
#include "H5Format.hpp"
#include "RingBuffer.hpp"
#include "ZmqReceiver.hpp"
#include <chrono>
#include "date.h"
#include "H5WriteModule.hpp"
#include "ZmqRecvModule.hpp"

class ProcessManager 
{
    H5WriteModule& write_module_;
    ZmqRecvModule& recv_module_;

    public:
        ProcessManager(H5WriteModule& write_module,
                       ZmqRecvModule& recv_module);


        void start(uint16_t rest_port);

};

#endif
