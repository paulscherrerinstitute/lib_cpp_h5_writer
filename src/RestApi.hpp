#ifndef REST_INTERFACE_H
#define REST_INTERFACE_H

#include "WriterManager.hpp"

namespace RestApi
{
    void start_rest_api(WriterManager& writer_manager, uint16_t port);
}

#endif