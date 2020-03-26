#include "gtest/gtest.h"
#include "BufferedWriter.hpp"
using namespace std;

TEST(BufferedWriter, get_buffered_writer)
{
    auto header_values = shared_ptr<unordered_map<string, HeaderDataType>>(new unordered_map<string, HeaderDataType> {
        {"frame", HeaderDataType("uint64")},
        {"module_number", HeaderDataType("uint64", 2)}
    });

    auto metadata_buffer1 = unique_ptr<MetadataBuffer>(new MetadataBuffer(1, header_values));

    auto dummy_writer = get_buffered_writer("/dev/null", 10, move(metadata_buffer1));
    EXPECT_TRUE(dynamic_cast<DummyBufferedWriter*>(dummy_writer.get()));

    auto metadata_buffer2 = unique_ptr<MetadataBuffer>(new MetadataBuffer(1, header_values));

    auto real_writer = get_buffered_writer("real_file.h5", 10, move(metadata_buffer2));
    EXPECT_FALSE(dynamic_cast<DummyBufferedWriter*>(real_writer.get()));
}

TEST(BufferedWriter, DummyBufferedWriter)
{
    DummyBufferedWriter dummy_writer;

    EXPECT_FALSE(dummy_writer.is_file_open());

    EXPECT_TRUE(dummy_writer.is_data_for_current_file(12345678));

    EXPECT_NO_THROW(dummy_writer.cache_metadata("test", 10, nullptr));

    EXPECT_NO_THROW(dummy_writer.write_metadata_to_file());

    EXPECT_NO_THROW(dummy_writer.close_file());

    EXPECT_THROW(dummy_writer.get_h5_file(), runtime_error);

    unique_ptr<char> buffer(new char[1]);
    vector<size_t> shape = {1};

    EXPECT_NO_THROW(dummy_writer.write_data("does not matter", 0, buffer.get(), shape, 0, "nop", "nop"));
}

TEST(BufferedWriter, process_manager_workflow)
{
    auto header_values = shared_ptr<unordered_map<string, HeaderDataType>>(new unordered_map<string, HeaderDataType> {
        {"frame", HeaderDataType("uint64")},
        {"module_number", HeaderDataType("uint64", 2)}
    });

    auto metadata_buffer = unique_ptr<MetadataBuffer>(new MetadataBuffer(1, header_values));
    auto writer = get_buffered_writer("/dev/null", 10, move(metadata_buffer));
    
    ASSERT_NO_THROW(writer->create_file());
    
    EXPECT_TRUE(writer->is_data_for_current_file(0));
    EXPECT_TRUE(writer->is_data_for_current_file(123456));

    // This 2 should not happen, because is_data_for_current_file should always return true.
    ASSERT_NO_THROW(writer->write_metadata_to_file());
    ASSERT_THROW(writer->get_h5_file(), runtime_error);

    unique_ptr<char> buffer(new char[1]);
    vector<size_t> shape = {1};
    EXPECT_NO_THROW(writer->write_data("does not matter", 0, buffer.get(), shape, 0, "nop", "nop"));

    EXPECT_NO_THROW(writer->cache_metadata("test", 10, nullptr));

    // This is important to prevent get_h5_file from being called.
    EXPECT_FALSE(writer->is_file_open());

    EXPECT_NO_THROW(writer->close_file());
}