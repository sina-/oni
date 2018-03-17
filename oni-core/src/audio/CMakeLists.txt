include_directories(${oni_SOURCE_DIR}/oni-core/inc)

add_library(oni-core-audio audio-manager.cpp)

target_link_libraries(oni-core-audio soloud SDL2)
