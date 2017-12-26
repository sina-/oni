# GLFW build

set(GLFW_RELEASE 3.2.1)
set(GLFW_INSTALL_DIR ${granite_SOURCE_DIR}/lib/glfw)
set(GLFW_INCLUDE_DIR ${GLFW_INSTALL_DIR}/include)
set(GLFW_LIB_DIR ${GLFW_INSTALL_DIR}/lib)

ExternalProject_Add(lib-glfw-${GLFW_RELEASE}
        GIT_REPOSITORY "https://github.com/glfw/glfw.git"
        GIT_TAG ${GLFW_RELEASE}
        UPDATE_COMMAND ""
        INSTALL_DIR "${GLFW_INSTALL_DIR}"
        CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_INSTALL_PREFIX=${GLFW_INSTALL_DIR}"
        -DGLFW_BUILD_EXAMPLES=OFF
        -DBUILD_SHARED_LIBS=OFF
        -DGLFW_BUILD_TESTS=OFF
        -DGLFW_BUILD_DOCS=OFF
        -DGLFW_INSTALL=ON
        )

add_library(glfw3 STATIC IMPORTED)
set_property(TARGET glfw3 PROPERTY IMPORTED_LOCATION "${GLFW_LIB_DIR}/libglfw3.a")
add_dependencies(glfw3 lib-glfw-${GLFW_RELEASE})
include_directories(${GLFW_INCLUDE_DIR})
link_directories(${GLFW_LIB_DIR})

#set(GLFW_RELEASE 3.2.1)
#ExternalProject_Add(lib-glfw-${GLFW_RELEASE}
#        GIT_REPOSITORY "https://github.com/glfw/glfw.git"
#        GIT_TAG ${GLFW_RELEASE}
#        DOWNLOAD_DIR ${granite_SOURCE_DIR}/download
#        DOWNLOAD_COMMAND git clone https://github.com/glfw/glfw.git
#        UPDATE_COMMAND ""
#        INSTALL_COMMAND ""
#        INSTALL_DIR ""
#        CMAKE_ARGS ""
#        BUILD_COMMAND ""
#        )
#add_subdirectory(${granite_SOURCE_DIR}/download/glfw)
#include_directories(${granite_SOURCE_DIR}/download/glfw/include)
