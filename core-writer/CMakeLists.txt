file(GLOB SOURCES
        src/*.cpp
        src/compression/*.cpp
        src/compression/*.c)

add_library(core-writer ${SOURCES})
include_directories(core-writer include/)

target_include_directories(core-writer PUBLIC src/)
target_include_directories(core-writer PUBLIC src/compression)
