FILE(GLOB_RECURSE SRC_FILES
        "Common/*.cpp"
        "Core/*.cpp"
        "Http/*.cpp"
)

add_library(http-lib ${SRC_FILES})

target_include_directories(http-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})