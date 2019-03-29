#ifndef RESTAPI_H
#define RESTAPI_H

#include "WriterManager.hpp"
#include <chrono>
#include "date.h"

namespace RestApi
{
    void start_rest_api(crow::SimpleApp app, WriterManager& writer_manager, uint16_t port);
}

#endif
