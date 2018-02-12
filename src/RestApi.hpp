#ifndef RESTAPI_H
#define RESTAPI_H

#include "WriterManager.hpp"

namespace RestApi
{
    void start_rest_api(WriterManager& writer_manager, uint16_t port);
}

#endif