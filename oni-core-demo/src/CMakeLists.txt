include_directories(${oni_SOURCE_DIR}/oni-core/inc)

add_executable(oni-core-demo main.cpp)
target_link_libraries(oni-core-demo
        # Not sure why this is needed, but bunch of GLFW linking fails without it
        ${CMAKE_DL_LIBS}
        ${X11_LIBRARY}
        ${GLFW_STATIC_LIBRARIES}
        ${OPENGL_LIBRARY}
        ${GLEW_STATIC_LIBRARIES}
        ${FREEIMAGE_LIBRARY}
        freetype-gl
        # NOTE: FreeType must be linked after freetype-gl because fucking G++
        ${FREETYPE_LIBRARY}
        gorilla
        ${OPENAL_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
        SDL2
        soloud
        oni-core-audio
        oni-core-buffers
        oni-core-entities
        oni-core-graphics
        oni-core-math
        oni-core-physics
        oni-core-utils)

if (UNIX)
    add_custom_command(TARGET oni-core-demo POST_BUILD
            COMMAND ln -sf ${oni_SOURCE_DIR}/oni-core/inc/shaders $<TARGET_FILE_DIR:oni-core-demo>)

    add_custom_command(TARGET oni-core-demo POST_BUILD
            COMMAND ln -sf ${oni_SOURCE_DIR}/resources $<TARGET_FILE_DIR:oni-core-demo>)
else ()
    # TODO: This solution requires manual re-build if shaders are updated
    # Copy the shaders from source tree to build tree
    # https://stackoverflow.com/a/18047175
    add_custom_command(TARGET oni-core-demo POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy_directory
            ${oni_SOURCE_DIR}/oni-core/inc/shaders
            $<TARGET_FILE_DIR:oni-core-demo>/shaders)
endif (UNIX)

IF (MSVC)
    # To pick up the working directory to where the executable is located.
    # Otherwise default working directory is one level above where it should be
    # https://stackoverflow.com/a/40937362
    set_target_properties(oni-core-demo PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}/Debug)
ENDIF ()