include_directories(${oni_SOURCE_DIR}/oni-core/inc)

add_library(oni-core-buffers buffer.cpp index-buffer.cpp vertex-array.cpp ../physics/movement.cpp)