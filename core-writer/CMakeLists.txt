add_subdirectory(external/)

file(GLOB SOURCES
        src/*.cpp
        src/writer/*.cpp
        src/receiver/*.cpp
        src/module/*.cpp)

add_library(core-writer STATIC ${SOURCES})
include_directories(core-writer external/)

target_include_directories(core-writer PUBLIC include/)
target_include_directories(core-writer PUBLIC external/)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(core-writer PRIVATE DEBUG_OUTPUT)
endif()

enable_testing()
add_subdirectory(test/)