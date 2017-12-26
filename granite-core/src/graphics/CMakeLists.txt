include_directories(${granite_SOURCE_DIR}/granite-core/inc)

add_library(granite-core-graphics window.cpp shader.cpp simple2drenderer.cpp)
add_dependencies(granite-core-graphics glfw3)
link_directories(${GLFW_LIB_DIR})
target_link_libraries(granite-core-graphics
        ${X11_LIBRARY}
        glfw3
        ${OPENGL_LIBRARY}
        glew
        )
