SET(prjName libeventSession)
set(srcS)
file(GLOB srcS src/*.cpp)
set(defS)
if (WIN32)
    MESSAGE(STATUS "windows")
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-w)
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W2)
	include_directories(
							${CMAKE_SOURCE_DIR}/../include
							$ENV{depInclude}
							)
	link_directories(
		${CMAKE_SOURCE_DIR}/../lib
		$ENV{depLib}
		)
	file(GLOB defS src/*.def)
endif ()

add_library(${prjName} SHARED ${srcS} ${defS})

target_include_directories(${prjName} PUBLIC 
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/cLog/src
                           )
target_link_libraries(${prjName} PUBLIC  common event)

if (WIN32)
	target_link_libraries(${prjName} PUBLIC ws2_32)
endif ()

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

