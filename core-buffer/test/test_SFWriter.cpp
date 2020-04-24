#include "SFWriter.hpp"
#include "gtest/gtest.h"

using namespace core_buffer;

TEST(SFWriter, basic_interaction)
{
    size_t n_modules = 2;
    size_t n_frames = 5;

    auto data = make_unique<uint16_t>(n_modules*MODULE_N_PIXELS);


    SFWriter writer("ignore.h5", 10, n_modules);
    writer.write(data, metadata);
    writer.close_file();


}