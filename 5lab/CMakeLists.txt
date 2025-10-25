cmake_minimum_required(VERSION 3.8)
project(net-app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

add_executable(net-app main.cpp)

target_link_libraries(net-app PRIVATE net-lib)