
#include "../src/H5Writer.hpp"
using namespace std;

TEST(H5Writer, get_h5_writer)
{
    auto dummy_writer = get_h5_writer("/dev/null");
    EXPECT_TRUE(dynamic_cast<DummyH5Writer*>(dummy_writer.get()));

    auto real_writer = get_h5_writer("real_file.h5");
    EXPECT_FALSE(dynamic_cast<DummyH5Writer*>(real_writer.get()));
}