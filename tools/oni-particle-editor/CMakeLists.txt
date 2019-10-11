cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(oni-particle-editor VERSION 0.1.0 LANGUAGES C CXX)

#set(CMAKE_CXX_FLAGS "-O3")

# TODO: Avoid relative path
set(oni_SOURCE_DIR ${oni-particle-editor_SOURCE_DIR}/../..)

subdirs(${oni_SOURCE_DIR}/src/asset)
subdirs(${oni_SOURCE_DIR}/src/entities)
subdirs(${oni_SOURCE_DIR}/src/graphic)
subdirs(${oni_SOURCE_DIR}/src/game)
subdirs(${oni_SOURCE_DIR}/src/utils)
subdirs(${oni_SOURCE_DIR}/src/physics)
subdirs(${oni_SOURCE_DIR}/src/math)
subdirs(${oni_SOURCE_DIR}/src/io)

subdirs(${oni-particle-editor_SOURCE_DIR}/src)

# add_subdirectory(${rac-common_SOURCE_DIR}/src/entities ${CMAKE_BINARY_DIR}/rac-common-entities)

add_executable(oni-particle-editor src/main.cpp)

target_compile_features(oni-particle-editor
        PUBLIC
        cxx_std_17
        )

target_link_libraries(oni-particle-editor
        PRIVATE
        oni-particle-editor-game
#        oni-particle-editor-graphic
#        oni-particle-editor-common-entities
#        oni-particle-editor-common-gameplay
#        oni-particle-editor-common-physics
        )

add_dependencies(oni-particle-editor
        oni-particle-editor-game
#        oni-particle-editor-graphic
#        oni-particle-editor-common-entities
#        oni-particle-editor-common-gameplay
#        oni-particle-editor-common-physics
        )

if (UNIX)
    add_custom_command(TARGET oni-particle-editor POST_BUILD
            COMMAND ln -sf ${oni_SOURCE_DIR}/resources $<TARGET_FILE_DIR:oni-particle-editor>/oni-resources)
else ()
    # TODO: This solution requires manual re-build if shaders are updated
    # Copy the shaders from source tree to build tree
    # https://stackoverflow.com/a/18047175
    add_custom_command(TARGET oni-particle-editor POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy_directory
            ${oni_SOURCE_DIR}/oni-core/inc/shaders
            $<TARGET_FILE_DIR:oni-particle-editor>/shaders)
endif (UNIX)

IF (MSVC)
    # To pick up the working directory to where the executable is located.
    # Otherwise default working directory is one level above where it should be
    # https://stackoverflow.com/a/40937362
    set_target_properties(oni-particle-editor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}/Debug)
ENDIF ()