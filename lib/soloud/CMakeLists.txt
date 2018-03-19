include_directories(${oni_SOURCE_DIR}/lib/soloud/include)

file(GLOB core ${oni_SOURCE_DIR}/lib/soloud/src/core/*.cpp)
file(GLOB filter ${oni_SOURCE_DIR}/lib/soloud/src/filter/*.cpp)
file(GLOB backend ${oni_SOURCE_DIR}/lib/soloud/src/backend/sdl2_static/*.cpp)
file(GLOB audiosource_wav ${oni_SOURCE_DIR}/lib/soloud/src/audiosource/wav/*.cpp)
file(GLOB audiosource_vorbis ${oni_SOURCE_DIR}/lib/soloud/src/audiosource/wav/*.c)

add_definitions("-D WITH_SDL2_STATIC")

add_library(soloud STATIC ${core} ${filter} ${backend} ${audiosource_wav} ${audiosource_vorbis})

target_link_libraries(soloud SDL2)
