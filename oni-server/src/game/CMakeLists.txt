add_library(oni-server-game server-game.cpp)

target_compile_features(oni-server-game
        PUBLIC
            cxx_std_17
        )

target_include_directories(oni-server-game
        PUBLIC
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        PRIVATE
            $<BUILD_INTERFACE:${oni-server_SOURCE_DIR}/inc>
        )

target_link_libraries(oni-server-game
        PRIVATE
            oni-core-buffers
            oni-core-entities
            oni-core-gameplay
            oni-core-game
            oni-core-graphics
            oni-core-math
            oni-core-io
            oni-core-network
            oni-core-physics
            oni-core-utils
        )

add_dependencies(oni-server-game
        oni-core-network
        )
