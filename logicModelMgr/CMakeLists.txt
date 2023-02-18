SET(prjName logicModelMgr)
set(srcS)
file(GLOB srcS src/*.cpp)
set(defS)
set(srcOS)

if (UNIX)
    MESSAGE(STATUS "unix")
	file(GLOB srcOS src/unix/*.cpp)
elseif (WIN32)
	MESSAGE(STATUS "windows")
	file(GLOB defS src/*.def)
	file(GLOB srcOS src/win/*.cpp)
	include_directories(
		${CMAKE_SOURCE_DIR}/../include
		)
	link_directories(
		${CMAKE_SOURCE_DIR}/../lib
		)
endif ()

add_library(${prjName} SHARED ${srcS} ${srcOS} ${defS})

target_include_directories(${prjName} PUBLIC 
							src
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/testLogicMsg/src
							${CMAKE_SOURCE_DIR}/logicCommon/src
							${CMAKE_SOURCE_DIR}/cLog/src
                           )
target_link_libraries(${prjName} PUBLIC  common logicCommon testLogicMsg)
#target_link_libraries(${prjName} PUBLIC  testLogicMsg)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
