include_directories(${oni_SOURCE_DIR}/inc)

add_library(oni-core-gameplay lap-tracker.cpp)

target_link_libraries(
        oni-core-gameplay
        oni-core-math
)
