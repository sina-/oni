# Partly inspired by https://stackoverflow.com/a/8304850
cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project (granite)

set (Granite_VERSION_MAJOR 0)
set (Granite_VERSION_MINOR 1)

# Project wide dependencies
find_package(OpenGL REQUIRED)

set(DEPENDS "${granite_SOURCE_DIR}/lib")

set(GLFW_DIR "${DEPENDS}/glfw-3.2.1.bin.WIN32")
set(GLEW_DIR "${DEPENDS}/glew-2.1.0")

include_directories(${GLFW_DIR}/include)
link_directories(${GLFW_DIR}/lib-vc2015)

include_directories(${GLEW_DIR}/include)
add_definitions(-DGLEW_STATIC)
link_directories(${GLEW_DIR}/lib/Release/Win32)

# Prject files
subdirs(granite-core granite-core-demo)
