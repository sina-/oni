add_library(oni-particle-editor-game oni-particle-editor-game.cpp)

target_compile_features(oni-particle-editor-game
        PUBLIC
        cxx_std_17
        )

target_include_directories(oni-particle-editor-game
        PUBLIC
        $<BUILD_INTERFACE:${oni-particle-editor_SOURCE_DIR}/inc>
        PRIVATE SYSTEM
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/anttweakbar/include>
        )

find_library(ANT_TWEAK_BAR AntTweakBar PATHS ${oni_SOURCE_DIR}/lib/anttweakbar/lib NO_DEFAULT_PATH)

target_link_libraries(oni-particle-editor-game
        PUBLIC
        oni-core-asset
        oni-core-buffer
        oni-core-entities
        oni-core-game
        oni-core-graphic
        oni-core-math
        oni-core-io
        oni-core-physics
        oni-core-utils
        PRIVATE
        ${ANT_TWEAK_BAR}
        )

add_dependencies(oni-particle-editor-game
        oni-core-asset
        oni-core-buffer
        oni-core-entities
        oni-core-game
        oni-core-graphic
        oni-core-math
        oni-core-io
        oni-core-physics
        oni-core-utils
        )
