add_library(oni-core-asset oni-asset-manager.cpp)

target_compile_features(oni-core-asset
        PUBLIC
        cxx_std_17
        )

target_include_directories(oni-core-asset
        PUBLIC
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        PRIVATE
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/cereal>
        )
