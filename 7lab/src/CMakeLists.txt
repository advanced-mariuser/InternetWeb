FILE(GLOB_RECURSE SRC_FILES
        "Common/*.cpp"
        "Core/*.cpp"
        "Mime/*.cpp"
)

add_library(smtp-lib ${SRC_FILES})

target_link_libraries(smtp-lib PRIVATE OpenSSL::SSL OpenSSL::Crypto)

target_include_directories(smtp-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})