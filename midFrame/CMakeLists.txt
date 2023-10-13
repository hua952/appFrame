SET(prjName midFrame)
set(srcS)
file(GLOB srcS src/*.cpp)
set(defS)
set(osSrc)
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
	include_directories(
		${CMAKE_SOURCE_DIR}/../include
		)
	link_directories(
		${CMAKE_SOURCE_DIR}/../lib
		)
endif ()

add_library(${prjName} SHARED ${srcS} ${osSrc} ${defS})
target_include_directories(${prjName} PUBLIC 
							src
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/logicCommon/src
							${CMAKE_SOURCE_DIR}/comTcpProNet/src
							${CMAKE_SOURCE_DIR}/cLog/src
                           )

target_link_libraries(${prjName} PUBLIC comTcpProNet logicCommon common)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

