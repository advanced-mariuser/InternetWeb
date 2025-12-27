FILE(GLOB_RECURSE SRC_FILES
        "Common/*.cpp"
        "Core/*.cpp"
        "Http/*.cpp"
        "Cache/*.cpp"
)

find_package(OpenSSL REQUIRED)

add_library(proxy-lib ${SRC_FILES})

target_link_libraries(proxy-lib PRIVATE OpenSSL::SSL OpenSSL::Crypto)

target_include_directories(proxy-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})