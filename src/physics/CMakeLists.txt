include_directories(${oni_SOURCE_DIR}/inc)

add_library(oni-core-physics dynamics.cpp car.cpp translation.cpp)

target_link_libraries(
        oni-core-physics
        oni-core-entities
        )