#include "gtest/gtest.h"
#include "test_ZmqReceiver.cpp"
#include "test_H5Writer.cpp"
#include "test_MetadataBuffer.cpp"
#include "test_BufferedWriter.cpp"
#include "test_ZmqRecvModule.cpp"
#include "test_H5WriteModule.cpp"
#include "test_ProcessManager.cpp"
#include "test_UdpReceiver.cpp"

using namespace std;

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
