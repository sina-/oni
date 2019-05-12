add_library(oni-server-entities oni-server-level-tile-world.cpp)

target_compile_features(oni-server-entities
        PUBLIC
        cxx_std_17
        )

target_include_directories(oni-server-entities
        PUBLIC
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        PRIVATE
        $<BUILD_INTERFACE:${oni-server_SOURCE_DIR}/inc>
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/box2d>
        )

find_library(BOX2D_LIBRARY Box2D PATHS ${oni_SOURCE_DIR}/lib/box2d/Build/bin/x86_64/Debug NO_DEFAULT_PATH)

target_link_libraries(oni-server-entities
        PRIVATE
        oni-core-math
        oni-core-physics
        oni-core-entities
        ${BOX2D_LIBRARY}
        )

add_dependencies(oni-server-entities
        oni-core-math
        oni-core-entities
        oni-core-physics
        )