add_library(oni-core-io input.cpp output.cpp)

target_compile_features(oni-core-io
        PUBLIC
            cxx_std_17
        )

target_include_directories(oni-core-io
        PUBLIC
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        )
