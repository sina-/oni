subdirs(${oni_SOURCE_DIR}/src/graphic/buffer)

add_library(oni-core-graphic window.cpp shader.cpp batch-renderer-2d.cpp font-manager.cpp renderer-2d.cpp scene-manager.cpp debug-draw-box2d.cpp texture-manager.cpp)

target_compile_features(oni-core-graphic
        PUBLIC
        cxx_std_17
        )

target_compile_definitions(oni-core-graphic
        PRIVATE
        GLEW_NO_GLU
        )

target_include_directories(oni-core-graphic
        PUBLIC
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/box2d>
        PRIVATE SYSTEM
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/freetype-gl/include>
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/freeimage/Source>
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/freetype2/include>
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/ant-tweak-bar/include>
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/glew/include>
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/glfw/include>
        )

find_library(ANT_TWEAK_BAR AntTweakBar PATHS ${oni_SOURCE_DIR}/lib/ant-tweak-bar/bin NO_DEFAULT_PATH)
find_library(FREETYPE_GL freetype-gl PATHS ${oni_SOURCE_DIR}/lib/freetype-gl/bin NO_DEFAULT_PATH)
find_library(FREETYPE_LIBRARY freetype PATHS ${oni_SOURCE_DIR}/lib/freetype2/build-cmake NO_DEFAULT_PATH)
find_library(FREEIMAGE_LIBRARY freeimage PATHS ${oni_SOURCE_DIR}/lib/freeimage NO_DEFAULT_PATH)
find_library(GLFW_LIBRARY glfw3 PATHS ${oni_SOURCE_DIR}/lib/glfw/build/src NO_DEFAULT_PATH)
find_library(GLEW_LIBRARY GLEW PATHS ${oni_SOURCE_DIR}/lib/glew/lib NO_DEFAULT_PATH)
find_library(BOX2D_LIBRARY Box2D PATHS ${oni_SOURCE_DIR}/lib/box2d/Build/bin/x86_64/Debug NO_DEFAULT_PATH)

set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

target_link_libraries(oni-core-graphic
        PRIVATE
        ${GLFW_LIBRARY}
        ${GLEW_LIBRARY}
        # Not sure why this is needed, but bunch of GLFW linking fails without it
        ${CMAKE_DL_LIBS}
        ${X11_LIBRARIES}
        ${FREEIMAGE_LIBRARY}
        ${FREETYPE_GL}
        # NOTE: FreeType must be linked after freetype-gl because fucking g++
        ${FREETYPE_LIBRARY}
        ${OPENGL_LIBRARY}
        ${ANT_TWEAK_BAR}
        ${BOX2D_LIBRARY}
        # NOTE: For some reason when building glfw from source these are not linked into the binary...
        Xinerama Xrandr Xcursor Xxf86vm
        oni-core-buffer
        oni-core-math
        oni-core-utils
        oni-core-entities
        )

add_dependencies(oni-core-graphic
        oni-core-buffer
        oni-core-math
        oni-core-utils
        )