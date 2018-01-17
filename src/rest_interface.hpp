#ifndef REST_INTERFACE_H
#define REST_INTERFACE_H

#include "WriterManager.hpp"
#include "h5_utils.hpp"

void start_rest_api(WriterManager& writer_manager, uint16_t port, std::map<std::string, DATA_TYPE>* input_value_type);

#endif