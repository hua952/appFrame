
SET(prjName libeventSession)
set(${prjName}Src)
file(GLOB ${prjName}Src *.cpp)
add_library(${prjName} SHARED ${${prjName}Src})
target_include_directories(${prjName} PUBLIC 
"../src/common/include" 
"../src/include" 
) 
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

target_link_libraries(${prjName} PUBLIC libevent)

