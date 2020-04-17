add_executable(core-buffer_tests test_main.cpp)

target_link_libraries(core-buffer_tests
        core-buffer
        gtest)