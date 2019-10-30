add_library(oni-core-entities
        oni-entities-serialization.cpp
        oni-entities-client-data-manager.cpp
        oni-entities-manager.cpp
        oni-entities-entity.cpp
        oni-entities-factory.cpp
        oni-entities-factory-client.cpp oni-entities-factory-server.cpp)

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
        oni-core-physics
        # TODO: I rather not have this dependencies, once I have figured out how entities
        # are initliazed for sync'ed and local types, then it would be obvoius if I have
        # to delay full initialization or not and then decide if this lib needs all these
        # dependencies to fully initialize entities
        oni-core-graphic
        PRIVATE
        ${BOX2D_LIBRARY}
        )
