add_executable(core-writer_tests test_main.cpp)

target_link_libraries(core-writer_tests
        core-writer
        external
        gtest
        zmq
        hdf5
        hdf5_hl
        hdf5_cpp)