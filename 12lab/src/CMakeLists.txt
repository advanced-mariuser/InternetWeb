FILE(GLOB_RECURSE SRC_FILES
        "Common/*.cpp"
        "Core/*.cpp"
        "Protocol/*.cpp"
)

add_library(rdtp-lib ${SRC_FILES})
target_include_directories(rdtp-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})