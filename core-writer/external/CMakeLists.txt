file(GLOB SOURCES bitshuffle/*.c)

add_library(external STATIC ${SOURCES})
target_include_directories(external PUBLIC .)
include_directories(/usr/include)