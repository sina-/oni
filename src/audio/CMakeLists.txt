add_library(oni-core-audio audio-manager-fmod.cpp)

target_compile_features(oni-core-audio
        PUBLIC
            cxx_std_17
        )

target_include_directories(oni-core-audio
        PUBLIC
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        PRIVATE
            ${oni_SOURCE_DIR}/lib/fmod/inc
        )

# You need to get the binary from https://fmod.com
find_library(FMOD fmodL PATHS ${oni_SOURCE_DIR}/lib/fmod/lib/x86_64 NO_DEFAULT_PATH)

target_link_libraries(oni-core-audio
        PRIVATE
            ${FMOD}
        )
