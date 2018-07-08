include_directories(${oni_SOURCE_DIR}/inc)

if (${AUDIO_MANAGER_SOLOUD})
    include_directories(${oni_SOURCE_DIR}/lib/soloud/include)
    # Required by soloud, even though documentation doesn't talk about it :/
    #set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    # Required by soloud
    # sudo apt install libsdl2-dev
    find_package(SDL2 REQUIRED)
    add_library(oni-core-audio audio-manager-soloud.cpp)

    set(AUDIO_LIBRARY
            # NOTE: Required by soloud, otherwise linker will complain with
            # /lib/x86_64-linux-gnu/libpthread.so.0: error adding symbols: DSO missing from command line
            ${CMAKE_THREAD_LIBS_INIT}
            soloud
            SDL2)

elseif (${AUDIO_MANAGER_FMOD})
    include_directories(SYSTEM ${oni_SOURCE_DIR}/lib/fmod/inc)

    # You need to get the binary from https://fmod.com
    find_library(FMOD fmodL PATHS ${oni_SOURCE_DIR}/lib/fmod/lib/x86_64)
    add_library(oni-core-audio audio-manager-fmod.cpp)
    set(AUDIO_LIBRARY ${FMOD})
endif ()

target_link_libraries(oni-core-audio
        ${AUDIO_LIBRARY}
        )
