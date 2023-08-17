SET(prjName common)
set(srcS)
file(GLOB srcS src/*.cpp)
set(osSrc)

set(libS)
MESSAGE(STATUS "to make common")
if (UNIX)
    MESSAGE(STATUS "unix")
	#add_definitions(-w)
	file(GLOB osSrc src/unix/*.cpp)
elseif (WIN32)
	file(GLOB osSrc src/win/*.cpp)
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W2)
	# list(APPEND libS ws2_32)
	# add_definitions(-DWIN32_LEAN_AND_MEAN)
	MESSAGE(STATUS "windows osSrc = ${osSrc}")
endif ()


add_library(${prjName} ${srcS} ${osSrc})

target_include_directories(${prjName} PUBLIC 
							src
                           )

if (WIN32)
	list(APPEND libS Dbghelp)
	target_link_libraries(${prjName} PUBLIC ${libS})
endif ()

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

install(TARGETS ${prjName} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
