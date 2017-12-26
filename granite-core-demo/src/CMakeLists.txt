include_directories(${granite_SOURCE_DIR}/granite-core/inc)
link_directories(${granite_SOURCE_DIR}/granite-core/src)

add_executable(granite-core-demo main.cpp)
target_link_libraries(granite-core-demo
    glfw3
    ${CMAKE_DL_LIBS}
    ${X11_LIBRARY}
	${OPENGL_LIBRARY}
    glew
    #${GLEW_LIBRARIES}
	granite-core-buffers
	granite-core-graphics
	granite-core-math
	granite-core-utils)

# Copy the shaders from source tree to build tree
# https://stackoverflow.com/a/18047175
add_custom_command(TARGET granite-core-demo POST_BUILD COMMAND 
	${CMAKE_COMMAND} -E copy_directory
	${granite_SOURCE_DIR}/granite-core/inc/shaders
	$<TARGET_FILE_DIR:granite-core-demo>/shaders
	)


IF (MSVC)
	# To pick up the working directory to where the executable is located.
	# Otherwise default working directory is one level above where it should be
	# https://stackoverflow.com/a/40937362
	set_target_properties(granite-core-demo PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY 
		${CMAKE_CURRENT_BINARY_DIR}/Debug)
ENDIF ()