SET(prjName comTcpProNet)
set(srcS)
file(GLOB srcS src/*.cpp)
if (UNIX)
	add_compile_options(-fPIC)
elseif (WIN32)
    MESSAGE(STATUS "windows")
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-w)
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W1)
endif ()

add_library(${prjName} ${srcS})
target_include_directories(${prjName} PUBLIC 
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/cLog/src
                           )

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

install(TARGETS ${prjName} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

set(incS src/comTcpNet.h)
install(FILES ${incS} DESTINATION include/${myProjectName})
