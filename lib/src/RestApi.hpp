#ifndef RESTAPI_H
#define RESTAPI_H

#include "WriterManager.hpp"
#include <chrono>
#include "date.h"

namespace RestApi
{
    extern const std::unordered_map<std::string, DATA_TYPE> rest_start_parameters;
    void start_rest_api(WriterManager& writer_manager, uint16_t port);
}

#endif
