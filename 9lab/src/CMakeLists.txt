FILE(GLOB_RECURSE SRC_FILES
        "Common/*.cpp"
        "Core/*.cpp"
        "Dns/*.cpp"
)

add_library(dns-lib ${SRC_FILES})

target_include_directories(dns-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})