cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(oni-test VERSION 0.1.0 LANGUAGES C CXX)

# TODO: Avoid relative path
set(oni_SOURCE_DIR ${oni-test_SOURCE_DIR}/..)

subdirs(${oni_SOURCE_DIR}/src/utils)
subdirs(${oni-test_SOURCE_DIR}/src)

add_executable(oni-test main.cpp)

target_include_directories(oni-test
        PRIVATE
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        $<BUILD_INTERFACE:${oni-test_SOURCE_DIR}/inc>
        )

target_compile_features(oni-test
        PUBLIC
        cxx_std_17
        )

target_link_libraries(oni-test
        oni-test-list
        )
