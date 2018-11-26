add_library(oni-core-network server.cpp client.cpp peer.cpp)

target_compile_features(oni-core-network
        PUBLIC
            cxx_std_17
        )

target_include_directories(oni-core-network
        PUBLIC
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        PRIVATE SYSTEM
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/enet/include>
        )

find_library(ENET_LIBRARY enet PATHS ${oni_SOURCE_DIR}/lib/enet/build NO_DEFAULT_PATH)

target_link_libraries(oni-core-network
        PRIVATE
            ${ENET_LIBRARY}
            oni-core-entities
        )

add_dependencies(oni-core-network
        oni-core-entities
        )
