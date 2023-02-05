SET(prjName common)
set(srcS)
file(GLOB srcS src/*.cpp)
set(osSrc)

MESSAGE(STATUS "to make common")
if (UNIX)
    MESSAGE(STATUS "unix")
	add_definitions(-w)
	file(GLOB osSrc src/unix/*.cpp)
elseif (WIN32)
	file(GLOB osSrc src/win/*.cpp)

	MESSAGE(STATUS "windows osSrc = ${osSrc}")
endif ()


add_library(${prjName} ${srcS} ${osSrc})

target_include_directories(${prjName} PUBLIC 
							src
                           )

if (WIN32)
	target_link_libraries(${prjName} PUBLIC Dbghelp)
endif ()

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
