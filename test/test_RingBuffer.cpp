#include "gtest/gtest.h"
#include "RingBuffer.hpp"

TEST(RingBufferTest, simple_operation)
{
    
    EXPECT_EQ(1000, 1000);	
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}