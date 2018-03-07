# Partly inspired by https://stackoverflow.com/a/8304850
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(oni)

set(CMAKE_CXX_STANDARD 17)
set(Oni_VERSION_MAJOR 0)
set(Oni_VERSION_MINOR 1)

#set(CMAKE_CXX_FLAGS "-O3")

#set(LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Project wide dependencies

find_package(PkgConfig REQUIRED)
#  sudo apt install libglfw3-dev
pkg_search_module(GLFW REQUIRED glfw3)
#  sudo apt install libglew-dev
pkg_search_module(GLEW REQUIRED glew)
include_directories(${GLFW_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIR})

#  sudo apt install libfreeimage3 libfreeimage-dev
find_library(FREEIMAGE_LIBRARY NAMES freeimage)

if(FREEIMAGE_LIBRARY)
    message("Found FreeImage at: ${FREEIMAGE_LIBRARY}")
else()
    message(FATAL_ERROR "Could not find FreeImage")
endif()

find_path(FREEIMAGE_INCLUDE_DIRS NAMES FreeImage.h)
if(FREEIMAGE_INCLUDE_DIRS)
    message("Found FreeImage header at: ${FREEIMAGE_INCLUDE_DIRS}")
    include_directories(${FREEIMAGE_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Could not find FreeImage header")
endif()

#  sudo apt install libfreetype6 libfreetype6-dev
find_library(FREETYPE_LIBRARY NAMES freetype)

if(FREETYPE_LIBRARY)
    message("Found FreeTypw at: ${FREETYPE_LIBRARY}")
else()
    message(FATAL_ERROR "Could not find FreeType")
endif()

find_path(FREETYPE_INCLUDE_DIRS NAMES ft2build.h PATH_SUFFIXES include/freetype2 include)
if(FREETYPE_INCLUDE_DIRS)
    message("Found FreeType header at: ${FREETYPE_INCLUDE_DIRS}")
    include_directories(${FREETYPE_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Could not find FreeType header")
endif()

# OpenGL is available through graphic cards driver
find_package(OpenGL REQUIRED)

find_package(X11 REQUIRED)
#link_libraries(${X11_LIBRARIES})

# Project files
subdirs(oni-core oni-core-demo)