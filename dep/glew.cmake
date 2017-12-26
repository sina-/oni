# GLEW build

set(GLEW_RELEASE glew-2.1.0)
set(GLEW_INSTALL_DIR ${granite_SOURCE_DIR}/lib/glew)
set(GLEW_INCLUDE_DIR ${GLEW_INSTALL_DIR}/include)
set(GLEW_LIB_DIR ${GLEW_INSTALL_DIR}/lib)
set(GLEW_LIB lib-${GLEW_RELEASE})

ExternalProject_Add(${GLEW_LIB}
        PREFIX ${GLEW_INSTALL_DIR}
        GIT_REPOSITORY https://github.com/nigels-com/glew.git
        GIT_TAG ${GLEW_RELESASE}
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND make extensions
        BUILD_IN_SOURCE true
        BUILD_COMMAND make
        INSTALL_COMMAND make install DESTDIR=${GLEW_INSTALL_DIR}
        CMAKE_ARGS
            ${PLATFORM_CMAKE_ARGS}
            ${TOOLCHAIN_CMAKE_ARGS}
            -DBUILD_UTILS=OFF
            -DBUILD_EXAMPLES=OFF
        "-DCMAKE_INSTALL_PREFIX=${GLEW_INSTALL_DIR}"
        )

add_library(glew STATIC IMPORTED)
set_property(TARGET glew PROPERTY IMPORTED_LOCATION "${granite_SOURCE_DIR}/lib/glew/usr/lib64/libGLEW.a")
add_dependencies(glew ${GLEW_LIB})
include_directories(${granite_SOURCE_DIR}/lib/glew/usr/include)
