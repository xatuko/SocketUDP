cmake_minimum_required(VERSION 3.0.0)
project(SocketUDP VERSION 0.1.0)

find_package(Threads REQUIRED)

add_executable(SocketUDP main.cpp)

add_subdirectory(libsockudp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

target_link_libraries(SocketUDP SocketUDP_library Threads::Threads)
