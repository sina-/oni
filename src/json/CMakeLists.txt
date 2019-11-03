add_library(oni-core-json
        oni-json.cpp
        )

target_compile_features(oni-core-json
        PUBLIC
        cxx_std_17
        )

target_include_directories(oni-core-json
        PUBLIC
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/lib/cereal>
        )

#target_link_libraries(oni-core-json
#        PUBLIC
#        PRIVATE
#        )
