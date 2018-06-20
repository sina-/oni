include_directories(${oni_SOURCE_DIR}/inc)
include_directories(${oni_SOURCE_DIR}/lib/freetype-gl/include)
# NOTE: SYSTEM so the warnings are disabled
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/entt)

# sudo apt install libfreeimage3 libfreeimage-dev
find_library(FREEIMAGE_LIBRARY NAMES freeimage)
if (FREEIMAGE_LIBRARY)
    message("Found FreeImage at: ${FREEIMAGE_LIBRARY}")
else ()
    message(FATAL_ERROR "Could not find FreeImage")
endif ()

find_path(FREEIMAGE_INCLUDE_DIRS NAMES FreeImage.h)
if (FREEIMAGE_INCLUDE_DIRS)
    message("Found FreeImage header at: ${FREEIMAGE_INCLUDE_DIRS}")
    include_directories(${FREEIMAGE_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Could not find FreeImage header")
endif ()

#  sudo apt install libfreetype6 libfreetype6-dev
find_library(FREETYPE_LIBRARY NAMES freetype)
if (FREETYPE_LIBRARY)
    message("Found FreeType at: ${FREETYPE_LIBRARY}")
else ()
    message(FATAL_ERROR "Could not find FreeType")
endif ()

find_path(FREETYPE_INCLUDE_DIRS NAMES ft2build.h PATH_SUFFIXES include/freetype2 include)
if (FREETYPE_INCLUDE_DIRS)
    message("Found FreeType header at: ${FREETYPE_INCLUDE_DIRS}")
    include_directories(${FREETYPE_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Could not find FreeType header")
endif ()

include_directories(${oni_SOURCE_DIR}/lib/freetype-gl/include)
find_library(FREETYPE_GL freetype-gl PATHS ${oni_SOURCE_DIR}/lib/freetype-gl/bin)

include_directories(${oni_SOURCE_DIR}/lib/ant-tweak-bar/include)
find_library(ANT_TWEAK_BAR AntTweakBar PATHS ${oni_SOURCE_DIR}/lib/ant-tweak-bar/bin)

# TODO: Statically link the dependencies by adding the lib to the repo together
# with headers
find_package(PkgConfig REQUIRED)
#  sudo apt install libglfw3-dev
pkg_search_module(GLFW REQUIRED glfw3)
#  sudo apt install libglew-dev
pkg_search_module(GLEW REQUIRED glew)

include_directories(${GLFW_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIR})

include_directories(${oni_SOURCE_DIR}/lib/Box2D)

find_package(OpenGL REQUIRED)

find_package(X11 REQUIRED)

add_library(oni-core-graphics window.cpp shader.cpp batch-renderer-2d.cpp texture.cpp font-manager.cpp renderer-2d.cpp scene-manager.cpp debug-draw-box2d.cpp)

# Note: without this line, linker will complain about missing function implementations
target_link_libraries(oni-core-graphics
        # Not sure why this is needed, but bunch of GLFW linking fails without it
        ${CMAKE_DL_LIBS}
        ${X11_LIBRARY}
        ${GLFW_STATIC_LIBRARIES}
        ${GLEW_STATIC_LIBRARIES}
        ${FREEIMAGE_LIBRARY}
        ${FREETYPE_GL}
        # NOTE: FreeType must be linked after freetype-gl because fucking g++
        ${FREETYPE_LIBRARY}
        ${OPENGL_LIBRARY}
        ${ANT_TWEAK_BAR}
        oni-core-buffers
        oni-core-io
        oni-core-entities
        )

add_dependencies(oni-core-graphics oni-core-buffers oni-core-io)