add_library(oni-core-buffers buffer.cpp index-buffer.cpp vertex-array.cpp)

target_compile_features(oni-core-buffers
        PUBLIC
            cxx_std_17
        )

target_compile_definitions(oni-core-buffers
        PRIVATE
            GLEW_NO_GLU
        )

find_library(GLFW_LIBRARY glfw3 PATHS ${oni_SOURCE_DIR}/lib/glfw/bin/x64/linux)
find_library(GLEW_LIBRARY GLEW PATHS ${oni_SOURCE_DIR}/lib/glew/lib)

target_include_directories(oni-core-buffers
        PUBLIC
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        PRIVATE
            ${oni_SOURCE_DIR}/lib/glew/include
        )

target_link_libraries(oni-core-buffers
        PUBLIC
            ${GLEW_LIBRARY}
        PRIVATE
            ${GLFW_LIBRARY}
        )