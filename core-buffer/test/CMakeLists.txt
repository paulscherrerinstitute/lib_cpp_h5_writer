add_executable(core-buffer_tests main.cpp)

target_link_libraries(core-buffer_tests
        core-buffer
        hdf5
        hdf5_hl
        hdf5_cpp
        gtest)