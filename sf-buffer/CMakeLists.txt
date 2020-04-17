add_executable(sf-buffer src/sf_buffer.cpp)

set_target_properties(sf-buffer PROPERTIES OUTPUT_NAME sf_buffer)
target_include_directories(sf-buffer PUBLIC src/)

target_link_libraries(sf-buffer
        core-buffer
        core-writer
        external
        zmq
        hdf5
        hdf5_hl
        hdf5_cpp
        boost_system
        pthread)

enable_testing()
add_subdirectory(test/)