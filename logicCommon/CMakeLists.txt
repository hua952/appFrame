SET(prjName logicCommon)
set(srcS)
file(GLOB srcS src/*.cpp)
add_library(${prjName} ${srcS})
target_include_directories(${prjName} PUBLIC 
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/cLog/src
                           )
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
