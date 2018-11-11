include_directories(${oni_SOURCE_DIR}/inc)

include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/Box2D)
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/entt)

add_library(oni-core-gameplay lap-tracker.cpp)

target_link_libraries(
        oni-core-gameplay
        oni-core-math
)
