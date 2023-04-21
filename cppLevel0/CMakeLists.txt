SET(prjName cppLevel0)
set(srcS)
file(GLOB srcS src/*.cpp)

set(osSrc)
if (UNIX)
    MESSAGE(STATUS "unix")
	#add_definitions(-w)
	#file(GLOB osSrc /src/unix/*.cpp)
elseif (WIN32)
    MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	#file(GLOB osSrc /src/win/*.cpp)
	#aux_source_directory(src/win osSrc)
endif ()

add_executable (${prjName} ${srcS} ${osSrc})
target_include_directories(${prjName} PUBLIC 
							src
							${CMAKE_SOURCE_DIR}/common/src
                           )

target_link_libraries(${prjName} PUBLIC  common)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
