include_directories(${oni_SOURCE_DIR}/oni-core/inc)

add_library(oni-core-graphics window.cpp shader.cpp batch-renderer-2d.cpp layer.cpp tilelayer.cpp texture.cpp)

# Note: without this line, linker will complain about missing function implementations
target_link_libraries(oni-core-graphics oni-core-buffers)