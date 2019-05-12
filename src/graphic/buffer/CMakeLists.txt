add_library(oni-core-buffer oni-graphic-buffer.cpp oni-graphic-index-buffer.cpp oni-graphic-vertex-array.cpp)

target_compile_features(oni-core-buffer
        PUBLIC
        cxx_std_17
        )

target_compile_definitions(oni-core-buffer
        PRIVATE
        GLEW_NO_GLU
        )

find_library(GLFW_LIBRARY glfw3 PATHS ${oni_SOURCE_DIR}/lib/glfw/bin/x64/linux NO_DEFAULT_PATH)
find_library(GLEW_LIBRARY GLEW PATHS ${oni_SOURCE_DIR}/lib/glew/lib NO_DEFAULT_PATH)

target_include_directories(oni-core-buffer
        PUBLIC
        $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        PRIVATE
        ${oni_SOURCE_DIR}/lib/glew/include
        )

target_link_libraries(oni-core-buffer
        PRIVATE
        ${GLEW_LIBRARY}
        ${GLFW_LIBRARY}
        )