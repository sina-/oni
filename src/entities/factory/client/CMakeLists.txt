add_library(oni-core-entities-factory-client
        oni-entities-factory-client.cpp
        )

target_compile_features(oni-core-entities-factory-client
        PUBLIC
        cxx_std_17
        )

target_include_directories(oni-core-entities-factory-client
        PUBLIC
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        )


target_link_libraries(oni-core-entities-factory-client
        PUBLIC
        oni-core-entities
        oni-core-graphic
        )
