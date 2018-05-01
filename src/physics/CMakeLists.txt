include_directories(${oni_SOURCE_DIR}/inc)
# NOTE: SYSTEM so the warnings are disabled
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/entt)

add_library(oni-core-physics dynamics.cpp car.cpp translation.cpp)

target_link_libraries(
        oni-core-physics
        oni-core-entities
        )