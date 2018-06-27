subdirs(${oni_SOURCE_DIR}/lib/enet)

include_directories(${oni_SOURCE_DIR}/inc)
# NOTE: SYSTEM so the warnings are disabled
include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/enet/include)

add_library(oni-core-network server.cpp)

target_link_libraries(
        oni-core-network
        enet
        )

add_dependencies(oni-core-network enet)
