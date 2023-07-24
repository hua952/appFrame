SET(prjName cLog)
set(srcS)
file(GLOB srcS src/*.cpp)
set(defS)
if (WIN32)
    MESSAGE(STATUS "windows")
	add_definitions(-w)
	include_directories(
							${CMAKE_SOURCE_DIR}/../include
							$ENV{depInclude}
							)
	link_directories(
		${CMAKE_SOURCE_DIR}/../lib
		)
endif ()

add_library(${prjName} ${srcS} )

target_include_directories(${prjName} PUBLIC 
							${CMAKE_SOURCE_DIR}/common/src
                           )
target_link_libraries(${prjName} PUBLIC  common
	)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
