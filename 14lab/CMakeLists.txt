cmake_minimum_required(VERSION 3.8)
project(proxy-server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

add_executable(proxy-server main.cpp)

target_link_libraries(proxy-server PRIVATE proxy-lib)