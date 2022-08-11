cmake_minimum_required(VERSION 3.16) 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 
# set the project name 
project(appFrame) 
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

SET(logicCommonName logicCommon)
set(logicCommonSrc)
file(GLOB logicCommonSrc src/logicCommon/*.cpp)
add_library(${logicCommonName} SHARED ${logicCommonSrc})
target_include_directories(${logicCommonName} PUBLIC 
"src/common/include" 
"src/include" 
)

SET(testLogicName testLogic)
set(testLogicSrc)
file(GLOB testLogicSrc src/testLogic/*.cpp)
add_library(${testLogicName} SHARED ${testLogicSrc})
target_include_directories(${testLogicName} PUBLIC 
"src/common/include" 
"src/testLogicMsg" 
"src/include" 
"src/logicCommon" 
) 
target_link_libraries(${testLogicName} PUBLIC  testLogicMsg  logicCommon)


SET(testLogicMsgName testLogicMsg)
set(testLogicMsgSrc)
file(GLOB testLogicMsgSrc src/testLogicMsg/*.cpp)
add_library(${testLogicMsgName} SHARED ${testLogicMsgSrc})
target_include_directories(${testLogicMsgName} PUBLIC 
"src/common/include" 
"src/include" 
"src/logicCommon" 
) 

add_subdirectory (cppLevel0L)

SET(cppLevel0Name cppLevel0)
set(cppLevel0Src)
file(GLOB cppLevel0Src src/cppLevel0/*.cpp)
set(cppLevel0SrcOS)
if (UNIX)
    MESSAGE(STATUS "unix")
	file(GLOB cppLevel0SrcOS src/cppLevel0/unix/*.cpp)
elseif (WIN32)
    MESSAGE(STATUS "windows")
endif ()

add_executable(${cppLevel0Name} ${cppLevel0Src} ${cppLevel0SrcOS})
target_include_directories(${cppLevel0Name} PUBLIC 
"src/include" 
"src/common/include" 
)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/bin)
target_link_libraries(${cppLevel0Name} PUBLIC ${midFrameName} commonL)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
