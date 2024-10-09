SET(prjName cppLevel0L)
set(srcS)
set(libDir ${firstBuildLib})
file(GLOB srcS src/*.cpp)
set(defS)
set(osSrc)
set(libS cLog comTcpProNet common frameConfig)

if (UNIX)
    MESSAGE(STATUS "unix")
	file(GLOB osSrc src/unix/*.cpp)
	add_compile_options(-fPIC)
elseif (WIN32)
	MESSAGE(STATUS "windows")
	add_definitions(-w)
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W2)
	ADD_DEFINITIONS(/Od)
	file(GLOB osSrc src/win/*.cpp)
	file(GLOB defS src/win/*.def)
endif ()

link_directories(
		${libDir}
		)

add_library(${prjName} SHARED ${srcS} ${defS} ${osSrc})
target_include_directories(${prjName} PUBLIC
							${CMAKE_SOURCE_DIR}/logicCommon/src
							${CMAKE_SOURCE_DIR}/frameConfig/src
							${firstBuildInc}
                           )
target_link_libraries(${prjName} PUBLIC  ${libS})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

