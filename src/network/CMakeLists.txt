subdirs(${oni_SOURCE_DIR}/lib/enet)

include_directories(${oni_SOURCE_DIR}/inc)
# NOTE: SYSTEM so the warnings are disabled
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/enet/include)
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/entt)
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/cereal)
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/Box2D)

add_library(oni-core-network server.cpp client.cpp peer.cpp)

target_link_libraries(
        oni-core-network
        oni-core-entities
        enet
        )

add_dependencies(oni-core-network enet oni-core-entities)
