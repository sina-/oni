include_directories(${oni_SOURCE_DIR}/lib/Box2D)

file(GLOB COLLISION ${oni_SOURCE_DIR}/lib/Box2D/Box2D/Collision/*.cpp)
file(GLOB SHAPES ${oni_SOURCE_DIR}/lib/Box2D/Box2D/Collision/Shapes/*.cpp)
file(GLOB COMMON ${oni_SOURCE_DIR}/lib/Box2D/Box2D/Common/*.cpp)
file(GLOB DYNAMICS ${oni_SOURCE_DIR}/lib/Box2D/Box2D/Dynamics/*.cpp)
file(GLOB CONTACTS ${oni_SOURCE_DIR}/lib/Box2D/Box2D/Dynamics/Contacts/*.cpp)
file(GLOB JOINTS ${oni_SOURCE_DIR}/lib/Box2D/Box2D/Dynamics/Joints/*.cpp)
file(GLOB ROPE ${oni_SOURCE_DIR}/lib/Box2D/Box2D/Rope/*.cpp)

add_library(box2d ${COLLISION} ${SHAPES} ${COMMON} ${DYNAMICS} ${CONTACTS} ${JOINTS} ${ROPE})