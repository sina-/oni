add_library(oni-server-game server-game.cpp)

target_compile_features(oni-server-game
        PUBLIC
            cxx_std_17
        )

target_include_directories(oni-server-game
        PUBLIC
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
            $<BUILD_INTERFACE:${oni-server_SOURCE_DIR}/inc>
        )

target_link_libraries(oni-server-game
        PRIVATE
            oni-core-entities
            oni-core-gameplay
            oni-core-game
            oni-core-network
            oni-core-physics
        )

add_dependencies(oni-server-game
        oni-core-network
        oni-core-physics
        oni-core-entities
        )
