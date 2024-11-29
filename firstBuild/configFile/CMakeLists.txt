SET(prjName configFile)
set(srcS)
file(GLOB srcS src/*.cpp)

find_path(RAPIDXML_INCLUDE_DIRS "rapidxml/rapidxml.hpp")
MESSAGE(STATUS "RAPIDXML_INCLUDE_DIRS : ${RAPIDXML_INCLUDE_DIRS} ")
set(osSrc)
if (UNIX)
    MESSAGE(STATUS "unix")
	add_definitions(-w)
elseif (WIN32)
    MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W1)
	include_directories(
							$ENV{depInclude}
							)
endif ()

add_executable (${prjName} ${srcS} ${osSrc})
target_include_directories(${prjName} PRIVATE 
							${CMAKE_SOURCE_DIR}/common/src
							PRIVATE ${RAPIDXML_INCLUDE_DIRS}
                           )
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
target_link_libraries(${prjName} PRIVATE  common)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS ${prjName} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

