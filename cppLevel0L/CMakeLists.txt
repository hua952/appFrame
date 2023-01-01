SET(prjName cppLevel0L)
set(srcS)
file(GLOB srcS src/*.cpp)
set(defS)
set(osSrc)
if (WIN32)
	MESSAGE(STATUS "windows")
	#add_definitions(-DUNICODE -D_UNICODE)
	file(GLOB defS src/*.def)
	file(GLOB osSrc src/win/*.cpp)
	include_directories(
		${CMAKE_SOURCE_DIR}/../include
		)
	link_directories(
		${CMAKE_SOURCE_DIR}/../lib
		)
endif ()

add_library(${prjName} SHARED ${srcS} ${defS} ${osSrc})
target_include_directories(${prjName} PUBLIC 
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/cLog/src
							src
                           )
					   target_link_libraries(${prjName} PUBLIC  midFrame cLog)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
