#include "gtest/gtest.h"
#include "../src/buffer_utils.hpp"

using namespace std;

TEST(BufferUtils, get_filename)
{
    auto expected_file = "/root/device-1/12300000/12345000.h5";

    auto root_folder = "/root";
    auto device_name = "device-1";

    auto result = get_filename(
            root_folder,
            device_name,
            12345000);

    ASSERT_EQ(result, expected_file);

    auto result2 = get_filename(
            root_folder,
            device_name,
            12345999);

    ASSERT_EQ(result2, expected_file);

    auto result3 = get_filename(
            root_folder,
            device_name,
            12346000);

    ASSERT_NE(result3, expected_file);

    auto result4 = get_filename(
            root_folder,
            device_name,
            12344999);

    ASSERT_NE(result4, expected_file);
}