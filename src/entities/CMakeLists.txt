add_library(oni-core-entities create-entity.cpp serialization.cpp client-data-manager.cpp)

target_compile_features(oni-core-entities
        PUBLIC
            cxx_std_17
        )

target_include_directories(oni-core-entities
        PUBLIC
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/entt/src>
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/cereal>
        PRIVATE
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/box2d>
        )

find_library(BOX2D_LIBRARY Box2D PATHS ${oni_SOURCE_DIR}/lib/box2d/Build/bin/x86_64/Debug NO_DEFAULT_PATH)

target_link_libraries(oni-core-entities
        PUBLIC
            oni-core-math
        PRIVATE
            ${BOX2D_LIBRARY}
            oni-core-physics
)

add_dependencies(oni-core-entities
        oni-core-physics
        )