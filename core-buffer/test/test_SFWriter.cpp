#include "SFWriter.hpp"
#include "gtest/gtest.h"

using namespace core_buffer;

TEST(SFWriter, basic_interaction)
{
    size_t n_modules = 2;
    size_t n_frames = 5;

    auto data = make_unique<char[]>(n_modules*MODULE_N_BYTES);
    auto metadata = make_shared<DetectorFrame>();

    SFWriter writer("ignore.h5", n_frames, n_modules);
    writer.write(data.get(), metadata);
    writer.close_file();

    // TODO: Write some test.
}