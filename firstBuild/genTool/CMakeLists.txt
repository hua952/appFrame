SET(prjName genTool)
set(srcS)
file(GLOB srcS src/*.cpp src/fromFileData/*.cpp src/procGen/*.cpp)

set(osSrc)
if (UNIX)
    MESSAGE(STATUS "unix")
	add_definitions(-w)
	file(GLOB osSrc /src/unix/*.cpp)
elseif (WIN32)
    MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W1)
	#file(GLOB osSrc /src/win/*.cpp)
	aux_source_directory(src/win osSrc)
	include_directories(
							$ENV{depInclude}
							)
endif ()

include_directories(
							${CMAKE_SOURCE_DIR}/../include
							)

add_executable (${prjName} ${srcS} ${osSrc})
target_include_directories(${prjName} PUBLIC 
							src
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/cLog/src
                           )
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
target_link_libraries(${prjName} PUBLIC  common cLog)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS ${prjName} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

