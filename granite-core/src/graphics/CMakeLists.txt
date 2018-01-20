include_directories(${granite_SOURCE_DIR}/granite-core/inc)

add_library(granite-core-graphics window.cpp shader.cpp simple2drenderer.cpp batchrenderer2d.cpp staticsprite.cpp)
