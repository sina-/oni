add_library(oni-core-game game.cpp)

target_compile_features(oni-core-game
        PUBLIC
            cxx_std_17
        )

find_package(Threads REQUIRED)

target_include_directories(oni-core-game
        PUBLIC
            $<BUILD_INTERFACE:${oni_SOURCE_DIR}/inc>
        )

target_link_libraries(oni-core-game
        PRIVATE
            # /lib/x86_64-linux-gnu/libpthread.so.0: error adding symbols: DSO missing from command line
            ${CMAKE_THREAD_LIBS_INIT}
        )