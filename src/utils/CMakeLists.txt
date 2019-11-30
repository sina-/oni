add_library(oni-core-utils oni-util-file.cpp)

target_compile_features(oni-core-utils
        PUBLIC
        cxx_std_17
        )

target_include_directories(oni-core-utils
        PUBLIC
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        )

target_link_libraries(
        oni-core-utils
        stdc++fs)
