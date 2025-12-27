cmake_minimum_required(VERSION 3.8)
project(dns-resolver)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

add_executable(dns-resolver main.cpp
        ../12lab/src/Protocol/Packet.h
        ../12lab/src/Protocol/UnreliableChannel.cpp
        ../12lab/src/Protocol/UnreliableChannel.h
        ../12lab/src/Core/RdtReceiver.cpp
        ../12lab/src/Core/RdtReceiver.h
        ../12lab/src/Core/RdtSender.cpp
        ../12lab/src/Core/RdtSender.h
        ../12lab/src/Common/UdpSocket.cpp
        ../12lab/src/Common/UdpSocket.h)

target_link_libraries(dns-resolver PRIVATE dns-lib)