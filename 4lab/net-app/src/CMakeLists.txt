FILE(GLOB_RECURSE SRC_FILES
        "Client/*.cpp"
        "Server/*.cpp"
        "Common/*.cpp"
)

FILE(GLOB_RECURSE HDR_FILES
        "Client/*.h"
        "Server/*.h"
        "Common/*.h"
)

add_library(net-lib ${SRC_FILES})

target_include_directories(net-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})