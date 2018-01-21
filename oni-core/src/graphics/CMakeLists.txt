include_directories(${oni_SOURCE_DIR}/oni-core/inc)

add_library(oni-core-graphics window.cpp shader.cpp simple2drenderer.cpp batchrenderer2d.cpp staticsprite.cpp)

# Note: without this line, linker will complain about missing function implementations
target_link_libraries(oni-core-graphics oni-core-buffers)
