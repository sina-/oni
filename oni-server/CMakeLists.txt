cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(oni-server VERSION 0.1.0 LANGUAGES C CXX)

# TODO: Avoid relative path
set(oni_SOURCE_DIR ${oni-server_SOURCE_DIR}/..)

subdirs(${oni_SOURCE_DIR}/src/network)
subdirs(${oni_SOURCE_DIR}/src/entities)
subdirs(${oni_SOURCE_DIR}/src/gameplay)
subdirs(${oni_SOURCE_DIR}/src/game)
subdirs(${oni_SOURCE_DIR}/src/utils)
subdirs(${oni_SOURCE_DIR}/src/physics)
subdirs(${oni_SOURCE_DIR}/src/math)
subdirs(${oni_SOURCE_DIR}/src/io)

add_subdirectory(${oni-server_SOURCE_DIR}/src/game)
add_subdirectory(${oni-server_SOURCE_DIR}/src/level)

add_executable(oni-server ${oni-server_SOURCE_DIR}/main.cpp)

target_compile_features(oni-server
        PUBLIC
            cxx_std_17
        )


target_include_directories(oni-server
        PRIVATE
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
            $<BUILD_INTERFACE:${oni-server_SOURCE_DIR}/inc>
        )

target_link_libraries(oni-server
        PRIVATE
            oni-server-game
            oni-core-network
        )

add_dependencies(oni-server
        oni-server-game
        oni-core-network
        )
