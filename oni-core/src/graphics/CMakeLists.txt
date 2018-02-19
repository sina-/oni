include_directories(${oni_SOURCE_DIR}/oni-core/inc)

add_library(oni-core-graphics window.cpp shader.cpp simple-2d-renderer.cpp batch-renderer2d.cpp layer.cpp tilelayer.cpp _car.cpp dynamic-tile-layer.cpp)

# Note: without this line, linker will complain about missing function implementations
target_link_libraries(oni-core-graphics oni-core-buffers)