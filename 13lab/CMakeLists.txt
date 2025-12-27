cmake_minimum_required(VERSION 3.8)
project(udp-pinger)

find_package(fmt REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

add_executable(udp_pinger_client main_client.cpp)
target_link_libraries(udp_pinger_client PRIVATE pinger-lib fmt::fmt)

add_executable(udp_pinger_server main_server.cpp)
target_link_libraries(udp_pinger_server PRIVATE pinger-lib fmt::fmt)