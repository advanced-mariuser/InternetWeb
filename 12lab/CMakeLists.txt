cmake_minimum_required(VERSION 3.8)
project(rdtp-protocol)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

add_executable(rdtp-sender main.cpp)
target_link_libraries(rdtp-sender PRIVATE rdtp-lib)

add_executable(rdt-receiver main_receiver.cpp)
target_link_libraries(rdt-receiver PRIVATE rdtp-lib)