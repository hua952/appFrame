SET(prjName midFrame)
set(srcS)
file(GLOB srcS src/*.cpp)
set(defS)
set(osSrc)
set(libDir ${firstBuildLib})
MESSAGE(STATUS "CMAKE_CXX_STANDARD = "${CMAKE_CXX_STANDARD})
if (UNIX)
    MESSAGE(STATUS "unix")
	file(GLOB osSrc src/unix/*.cpp)
	add_compile_options(-fPIC)
elseif (WIN32)
	MESSAGE(STATUS "windows")
	add_definitions(-w)
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W1)
	ADD_DEFINITIONS(/Od)
	file(GLOB osSrc src/win/*.cpp)
	file(GLOB defS src/win/*.def)
endif ()

link_directories(
		${libDir}
		)
add_library(${prjName} SHARED ${srcS} ${osSrc} ${defS})
target_include_directories(${prjName} PUBLIC 
							${CMAKE_SOURCE_DIR}/logicCommon/src
							${firstBuildInc}
                           )

target_link_libraries(${prjName} PUBLIC comTcpProNet logicCommon common)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

