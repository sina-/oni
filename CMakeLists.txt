# Partly inspired by https://stackoverflow.com/a/8304850
cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project (oni)

set(CMAKE_CXX_STANDARD 17)
set(Oni_VERSION_MAJOR 0)
set(Oni_VERSION_MINOR 1)
#set(LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Project wide dependencies

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)
include_directories(${GLFW_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIR})

# OpenGL is available through graphic cards driver
find_package(OpenGL REQUIRED)

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})

# Project files
subdirs(oni-core oni-core-demo)