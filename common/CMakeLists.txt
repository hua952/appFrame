SET(prjName common)
set(srcS)
file(GLOB srcS src/*.cpp)

add_library(${prjName} ${srcS} )

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
