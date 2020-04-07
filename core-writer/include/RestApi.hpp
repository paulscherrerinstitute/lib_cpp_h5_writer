#ifndef RESTAPI_H
#define RESTAPI_H


#include <chrono>
#include "date.h"
#include "ProcessManager.hpp"

namespace RestApi
{
    void start_rest_api(ProcessManager& manager, uint16_t port);
}

#endif
