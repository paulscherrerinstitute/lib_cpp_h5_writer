file(GLOB SOURCES
        *.cpp)

add_executable(sf-buffer sf_buffer.cpp)
set_target_properties(sf-buffer PROPERTIES OUTPUT_NAME sf_buffer)

target_link_libraries(sf-buffer
        core-writer
        external
        zmq
        hdf5
        hdf5_hl
        hdf5_cpp
        boost_system
        pthread)
