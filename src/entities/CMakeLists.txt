include_directories(${oni_SOURCE_DIR}/inc)

include_directories(${oni_SOURCE_DIR}/lib/freetype-gl/include)
# NOTE: SYSTEM so the warnings are disabled
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/entt)
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/cereal)
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/Box2D)

add_library(oni-core-entities create-entity.cpp manage-hierarchy.cpp tile-world.cpp serialization.cpp client-data-manager.cpp)

target_link_libraries(
        oni-core-entities
        oni-core-graphics
        oni-core-math

)

add_dependencies(
        oni-core-entities
        oni-core-graphics
        oni-core-math)