cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(oni-server C CXX)

set(CMAKE_CXX_STANDARD 17)
set(Oni_VERSION_MAJOR 0)
set(Oni_VERSION_MINOR 1)

# TODO: Avoid relative path
set(oni_SOURCE_DIR ${oni-server_SOURCE_DIR}/..)

add_subdirectory(${oni_SOURCE_DIR}/src/network ${CMAKE_CURRENT_BINARY_DIR}/oni-core)
#subdirs(${oni_SOURCE_DIR}/src/network)

include_directories(${oni_SOURCE_DIR}/inc)
include_directories(${oni_SOURCE_DIR}/lib/enet/include)

add_executable(oni-server ${oni-server_SOURCE_DIR}/main.cpp)

target_link_libraries(oni-server
        oni-core-network)

add_dependencies(oni-server oni-core-network)
