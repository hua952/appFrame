SET(prjName cppLevel0)
set(srcS)
file(GLOB srcS src/*.cpp)

set(libS common)
set(osSrc)
if (UNIX)
    MESSAGE(STATUS "unix")
	#add_definitions(-w)
	file(GLOB osSrc src/unix/*.cpp)
elseif (WIN32)
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W2)
	list(APPEND libS ws2_32)
	file(GLOB osSrc src/win/*.cpp)
	MESSAGE(STATUS "${prjName} windows osSrc = ${osSrc}")
	#aux_source_directory(src/win osSrc)
endif ()

add_executable (${prjName} ${srcS} ${osSrc})
target_include_directories(${prjName} PUBLIC 
							src
							${CMAKE_SOURCE_DIR}/common/src
                           )

target_link_libraries(${prjName} PUBLIC  ${libS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

install(TARGETS ${prjName} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

