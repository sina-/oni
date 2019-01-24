add_library(oni-core-physic dynamics.cpp car.cpp transformation.cpp collision.cpp projectile.cpp)

target_compile_features(oni-core-physic
        PUBLIC
            cxx_std_17
        )

target_include_directories(oni-core-physic
        PUBLIC
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        PRIVATE SYSTEM
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/cereal>
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/entt>
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/box2d>
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/glfw/include>
        )

find_library(BOX2D_LIBRARY Box2D PATHS ${oni_SOURCE_DIR}/lib/box2d/Build/bin/x86_64/Debug NO_DEFAULT_PATH)

target_link_libraries(oni-core-physic
        PRIVATE
            ${BOX2D_LIBRARY}
            oni-core-entities
            oni-core-math
            oni-core-io
        )

add_dependencies(oni-core-physic
        oni-core-io
        oni-core-math
        )
