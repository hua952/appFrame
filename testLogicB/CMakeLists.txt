SET(prjName testLogicB)
set(srcS)
file(GLOB srcS src/*.cpp)
set(defS)
if (WIN32)
	MESSAGE(STATUS "windows")
	#add_definitions(-DUNICODE -D_UNICODE)
	file(GLOB defS src/*.def)
	include_directories(
		${CMAKE_SOURCE_DIR}/../include
		)
	link_directories(
		${CMAKE_SOURCE_DIR}/../lib
		)
endif ()


add_library(${prjName} SHARED ${srcS} ${defS})

target_include_directories(${prjName} PUBLIC 
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/testLogicMsg/src
							${CMAKE_SOURCE_DIR}/logicCommon/src
							${CMAKE_SOURCE_DIR}/cLog/src
                           )
target_link_libraries(${prjName} PUBLIC  common)
target_link_libraries(${prjName} PUBLIC  testLogicMsg)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
