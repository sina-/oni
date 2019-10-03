add_library(oni-core-game oni-game.cpp)

target_compile_features(oni-core-game
        PUBLIC
        cxx_std_17
        )

target_include_directories(oni-core-game
        PUBLIC
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        )
