add_library(oni-particle-editor-entities-factory
        oni-particle-editor-entities-factory.cpp
        )

target_compile_features(oni-particle-editor-entities-factory
        PUBLIC
        cxx_std_17
        )

target_include_directories(oni-particle-editor-entities-factory
        PUBLIC
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        $<BUILD_INTERFACE:${oni-particle-editor_SOURCE_DIR}/inc>
        )

target_link_libraries(oni-particle-editor-entities-factory
        PUBLIC
        oni-core-entities
        oni-core-entities-factory-client
        )
