cmake_minimum_required(VERSION 3.8)
project(smtp-client-pro)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenSSL REQUIRED)

add_subdirectory(src)

add_executable(smtp-client-pro main.cpp
        ../9lab/src/Dns/DnsMessage.cpp
        ../9lab/src/Dns/DnsMessage.h
        ../9lab/src/Core/Resolver.cpp
        ../9lab/src/Core/Resolver.h
        ../9lab/src/Common/Socket.cpp
        ../9lab/src/Common/Socket.h)

target_link_libraries(smtp-client-pro PRIVATE smtp-lib)