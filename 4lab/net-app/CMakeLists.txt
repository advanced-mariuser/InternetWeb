cmake_minimum_required(VERSION 3.8)
project(net-app)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

add_executable(net-app main.cpp
        ../../5lab/src/Common/Socket.cpp
        ../../5lab/src/Common/Socket.h
        ../../5lab/src/Core/Server.cpp
        ../../5lab/src/Core/Server.h
        ../../5lab/src/Http/HttpRequest.cpp
        ../../5lab/src/Http/HttpRequest.h
        ../../5lab/src/Http/HttpResponse.cpp
        ../../5lab/src/Http/HttpResponse.h)

target_link_libraries(net-app PRIVATE net-lib)