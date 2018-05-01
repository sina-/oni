include_directories(${oni_SOURCE_DIR}/inc)

include_directories(${oni_SOURCE_DIR}/lib/freetype-gl/include)
include_directories(${oni_SOURCE_DIR}/lib/entt)

add_library(oni-core-entities create-entity.cpp)

target_link_libraries(
        oni-core-entities
        oni-core-graphics
        oni-core-math

)

add_dependencies(
        oni-core-entities
        oni-core-graphics
        oni-core-math)