#include "gtest/gtest.h"
#include "../src/H5Writer.hpp"
using namespace std;

TEST(H5Writer, get_h5_writer)
{
    auto dummy_writer = get_h5_writer("/dev/null");
    EXPECT_TRUE(dynamic_cast<DummyH5Writer*>(dummy_writer.get()));

    auto real_writer = get_h5_writer("real_file.h5");
    EXPECT_FALSE(dynamic_cast<DummyH5Writer*>(real_writer.get()));
}

TEST(H5Writer, DummyH5Writer)
{
    DummyH5Writer dummy_writer;

    EXPECT_FALSE(dummy_writer.is_file_open());

    EXPECT_NO_THROW(dummy_writer.close_file());

    EXPECT_THROW(dummy_writer.get_h5_file(), runtime_error);

    unique_ptr<char> buffer(new char[1]);
    vector<size_t> shape = {1};

    EXPECT_NO_THROW(dummy_writer.write_data("does not matter", 0, buffer.get(), shape, 0, "nop", "nop"));
}