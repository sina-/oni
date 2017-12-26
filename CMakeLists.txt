# Partly inspired by https://stackoverflow.com/a/8304850
cmake_minimum_required (VERSION 2.6 FATAL_ERROR)
project (granite)

set(CMAKE_CXX_STANDARD 14)
set(Granite_VERSION_MAJOR 0)
set(Granite_VERSION_MINOR 1)
set(LIBDIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

include(ExternalProject)

# Project wide dependencies

# Build dependencies
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/dep")
include(glfw)
include(glew)

# OpenGL is available through graphic cards driver
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
find_package(XRandr REQUIRED)
find_package(xi REQUIRED)
find_package(xxf86vm REQUIRED)

link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})
link_libraries(${OPENGL_LIBRARIES})
include_directories(${OPENGL_INCLUDE_DIR})

#find_package(GLEW REQUIRED)
#if(GLEW_FOUND)
#    message("Glew found!")
#    message("${GLEW_INCLUDE_DIRS}")
#    message("${GLEW_LIBRARIES}")
#    include_directories(${GLEW_INCLUDE_DIRS})
#    add_definitions(-DWITH_GLEW)
#    link_directories(${GLEW_LIBRARY_DIRS})
#    message("${GLEW_LIBRARY_DIRS}")
#endif()

#set(DEPENDS "${granite_SOURCE_DIR}/lib")
# TODO hardcoded to WIN32
#set(GLFW_DIR "${DEPENDS}/glfw-3.2.1.bin.WIN32")
#set(GLEW_DIR "${DEPENDS}/glew-2.1.0")

#ExternalProject_Add(
#        GLFW
#        GIT_REPOSITORY https://github.com/glfw/glfw.git
#        GIT_TAG 3.2.1
#        CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/_my_install"
#)
#include_directories(${GLFW_DIR}/include)
#link_directories(${GLFW_DIR}/lib-vc2015)

# Use built binaries on WIN32, otherwise build from source
# glew.sourceforge.net/install.html
#if(WIN32)
    #include_directories(${GLEW_DIR}/include)
    #add_definitions(-DGLEW_STATIC)
    #link_directories(${GLEW_DIR}/lib/Release/Win32)
#else()
    #subdirs(lib)
#endif()


#ExternalProject_Add(
#        project_glew
#        URL https://github.com/nigels-com/glew/archive/glew-2.1.0.zip
#        DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/downloads
#        PREFIX "${CMAKE_CURRENT_BINARY_DIR}/glew"
#        CONFIGURE_COMMAND ""
#        BUILD_IN_SOURCE 1
#        BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} glew.lib
#        INSTALL_COMMAND ""
#)
#
#ExternalProject_Get_Property(project_glew SOURCE_DIR)
#set(GLEW_SOURCE_DIR "${SOURCE_DIR}/src")
#
#add_library(glew STATIC IMPORTED)
#set_property(TARGET glew PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libGLEW.a")
#add_dependencies(glew project_glew)

#set(GLEW_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/include")
#set(GLEW_LIBRARY "glew")

# Project files
subdirs(granite-core granite-core-demo)