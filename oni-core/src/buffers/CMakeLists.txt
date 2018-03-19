include_directories(${oni_SOURCE_DIR}/oni-core/inc)

# TODO: Statically link the dependencies by adding the lib to the repo together
# with headers
find_package(PkgConfig REQUIRED)
#  sudo apt install libglfw3-dev
pkg_search_module(GLFW REQUIRED glfw3)
#  sudo apt install libglew-dev
pkg_search_module(GLEW REQUIRED glew)
include_directories(${GLFW_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIR})

add_library(oni-core-buffers buffer.cpp index-buffer.cpp vertex-array.cpp)

target_link_libraries(oni-core-buffers
        ${GLFW_STATIC_LIBRARIES}
        ${GLEW_STATIC_LIBRARIES}
        )