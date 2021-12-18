cmake_minimum_required(VERSION 3.16) 
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 
# set the project name 
project(mainLoop) 
set(commonLSrc)
file(GLOB commonLSrc src/common/*.cpp)
add_library(commonL SHARED ${commonLSrc})
target_include_directories(commonL PUBLIC 
                           "src/common/include" 
                           ) 

SET(midFrameName midFrame)
set(midFrameSrc)
file(GLOB midFrameSrc src/midFrame/*.cpp)
set(midFrameSrcOS)
if (UNIX)
    MESSAGE(STATUS "unix")
	file(GLOB midFrameSrcOS src/midFrame/unix/*.cpp)
elseif (WIN32)
    MESSAGE(STATUS "windows")
endif ()
add_library(${midFrameName} SHARED 
	${midFrameSrc}
	${midFrameSrcOS}
	)

target_include_directories(midFrame PUBLIC 
                           "src/common/include" 
                           "src/include" 
						   "src/midFrame" 
                           ) 
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

