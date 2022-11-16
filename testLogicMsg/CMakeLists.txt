SET(prjName testLogicMsg)
set(srcS)
file(GLOB srcS src/*.cpp)
add_library(${prjName} ${srcS})

target_include_directories(${prjName} PUBLIC 
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/logicCommon/src
                           )
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
