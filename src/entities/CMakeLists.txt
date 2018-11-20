add_library(oni-core-entities create-entity.cpp manage-hierarchy.cpp tile-world.cpp serialization.cpp client-data-manager.cpp)

target_compile_features(oni-core-entities
        PUBLIC
            cxx_std_17
        )

target_include_directories(oni-core-entities
        PUBLIC
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/entt>
        PRIVATE
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/cereal>
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/box2d>
        )

target_link_libraries(oni-core-entities
        PRIVATE
            oni-core-graphics
            oni-core-physics
            oni-core-math
            oni-core-io
)

add_dependencies(oni-core-entities
        oni-core-graphics
        oni-core-math
        oni-core-io
        )