FILE(GLOB_RECURSE SRC_FILES
        "Common/*.cpp"
        "Client/*.cpp"
        "Server/*.cpp"
)

add_library(pinger-lib ${SRC_FILES})

target_include_directories(pinger-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})