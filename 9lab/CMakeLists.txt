cmake_minimum_required(VERSION 3.8)
project(dns-resolver)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

add_executable(dns-resolver main.cpp)

target_link_libraries(dns-resolver PRIVATE dns-lib)