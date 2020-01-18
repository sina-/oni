add_library(oni-test-list
        oni-test-enum.cpp
        oni-test-enum-storage-a.cpp
        oni-test-enum-storage-b.cpp
        oni-test-enum-storage-c.cpp)

target_compile_features(oni-test-list
        PUBLIC
        cxx_std_17
        )

target_include_directories(oni-test-list
        PUBLIC
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        $<BUILD_INTERFACE:${oni-test_SOURCE_DIR}/inc>
        )
