cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(oni-server C CXX)

set(CMAKE_CXX_STANDARD 17)
set(Oni_VERSION_MAJOR 0)
set(Oni_VERSION_MINOR 1)

# TODO: Avoid relative path
set(oni_SOURCE_DIR ${oni-server_SOURCE_DIR}/..)

#add_subdirectory(${oni_SOURCE_DIR}/src/network ${CMAKE_CURRENT_BINARY_DIR}/oni-core)
#add_subdirectory(${oni_SOURCE_DIR}/src/entities ${CMAKE_CURRENT_BINARY_DIR}/oni-core)

subdirs(${oni_SOURCE_DIR}/src/network)
subdirs(${oni_SOURCE_DIR}/src/entities)
subdirs(${oni_SOURCE_DIR}/src/gameplay)
subdirs(${oni_SOURCE_DIR}/src/graphics)
subdirs(${oni_SOURCE_DIR}/src/game)
subdirs(${oni_SOURCE_DIR}/src/utils)
subdirs(${oni_SOURCE_DIR}/src/physics)
subdirs(${oni_SOURCE_DIR}/src/math)
subdirs(${oni_SOURCE_DIR}/src/buffers)
subdirs(${oni_SOURCE_DIR}/src/io)

include_directories(${oni_SOURCE_DIR}/inc)
include_directories(${oni-server_SOURCE_DIR}/inc)
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/enet/include)
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/cereal)
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/Box2D)
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/entt)

subdirs(${oni-server_SOURCE_DIR}/src)

add_executable(oni-server ${oni-server_SOURCE_DIR}/main.cpp)

target_link_libraries(oni-server
        oni-server-game
        oni-core-network)

add_dependencies(oni-server oni-server-game oni-core-network)
