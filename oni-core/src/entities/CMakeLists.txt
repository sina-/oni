include_directories(${oni_SOURCE_DIR}/oni-core/inc)

include_directories(${oni_SOURCE_DIR}/lib/freetype-gl/include)

add_library(oni-core-entities world.cpp create-entity.cpp)
