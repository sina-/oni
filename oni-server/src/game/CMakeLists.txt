include_directories(${oni-server_SOURCE_DIR}/inc)

add_library(oni-server-game server-game.cpp)

target_link_libraries(oni-server-game
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
