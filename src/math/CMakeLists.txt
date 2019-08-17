add_library(oni-core-math
        oni-math-mat4.cpp
        oni-math-vec2.cpp
        oni-math-vec3.cpp
        oni-math-vec4.cpp
        oni-math-intersects.cpp
        oni-math-rand.cpp
        oni-math-z-layer-manager.cpp
        oni-math-function.cpp
        oni-math-transformation.cpp)

target_compile_features(oni-core-math
        PUBLIC
        cxx_std_17
        )

target_include_directories(oni-core-math
        PUBLIC
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        )
