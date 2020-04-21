find_package(MPI REQUIRED)

file(GLOB SOURCES
        *.cpp)

add_executable(sf-writer sf_h5_writer.cpp)
set_target_properties(sf-writer PROPERTIES OUTPUT_NAME sf_h5_writer)

target_include_directories(sf-writer PRIVATE ${MPI_CXX_INCLUDE_PATH})
target_compile_options(sf-writer PRIVATE ${MPI_CXX_COMPILE_FLAGS})

target_link_libraries(sf-writer
        ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS}
        core-writer
        external
        zmq
        hdf5
        hdf5_hl
        hdf5_cpp
        boost_system
        pthread)
