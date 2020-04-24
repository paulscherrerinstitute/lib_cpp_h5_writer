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

add_executable(sf-udprecv src/sf_udprecv.cpp)
set_target_properties(sf-udprecv PROPERTIES OUTPUT_NAME sf_udprecv)

target_link_libraries(sf-udprecv
        core-buffer
        core-writer
        external
        zmq
        hdf5
        hdf5_hl
        hdf5_cpp
        boost_system
        pthread)

add_executable(sf-reader src/sf_reader.cpp)
set_target_properties(sf-reader PROPERTIES OUTPUT_NAME sf_reader)

target_link_libraries(sf-reader
        core-buffer
        core-writer
        external
        zmq
        hdf5
        hdf5_hl
        hdf5_cpp
        boost_system
        pthread)

add_executable(sf-replay src/sf_replay.cpp)
set_target_properties(sf-replay PROPERTIES OUTPUT_NAME sf_replay)

target_link_libraries(sf-replay
        core-buffer
        core-writer
        external
        zmq
        hdf5
        hdf5_hl
        hdf5_cpp
        boost_system
        pthread)

add_executable(sf-writer src/sf_replay.cpp)
set_target_properties(sf-writer PROPERTIES OUTPUT_NAME sf_writer)

target_link_libraries(sf-writer
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