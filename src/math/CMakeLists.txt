add_library(oni-core-math mat4.cpp vec2.cpp vec3.cpp vec4.cpp intesects.cpp rand.cpp z-layer-manager.cpp)

target_compile_features(oni-core-math
        PUBLIC
            cxx_std_17
        )

target_include_directories(oni-core-math
        PUBLIC
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        )
