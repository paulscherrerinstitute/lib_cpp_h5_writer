file(GLOB SOURCES
        *.cpp)

add_executable(sf-writer sf_h5_writer.cpp)
set_target_properties(sf-writer PROPERTIES OUTPUT_NAME sf_h5_writer)

target_link_libraries(sf-writer
        core-writer
        external
        zmq
        hdf5
        hdf5_hl
        hdf5_cpp
        boost_system
        pthread)
