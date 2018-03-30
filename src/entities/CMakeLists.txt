include_directories(${oni_SOURCE_DIR}/inc)

include_directories(${oni_SOURCE_DIR}/lib/freetype-gl/include)

add_library(oni-core-entities basic-entity-repo.cpp create-entity.cpp vehicle.cpp)
