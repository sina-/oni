add_library(oni-core-gameplay lap-tracker.cpp)

target_compile_features(oni-core-gameplay
        PUBLIC
            cxx_std_17
        )

target_include_directories(oni-core-gameplay
        PUBLIC
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        )

target_link_libraries(oni-core-gameplay
        PRIVATE
            oni-core-math
            oni-core-entities
)

add_dependencies(oni-core-gameplay
        oni-core-entities
        )
