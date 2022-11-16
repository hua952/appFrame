SET(prjName midFrame)
set(srcS)
file(GLOB srcS src/*.cpp)

set(defS)
set(osSrc)
if (UNIX)
    MESSAGE(STATUS "unix")
	file(GLOB osSrc /src/unix/*.cpp)
elseif (WIN32)
    MESSAGE(STATUS "windows")
	#file(GLOB osSrc /src/win/*.cpp)
	aux_source_directory(src/win osSrc)
	file(GLOB defS src/win/*.def)
endif ()

add_library(${prjName} SHARED ${srcS} ${osSrc} ${defS})
target_include_directories(${prjName} PUBLIC 
							src
							${CMAKE_SOURCE_DIR}/common/src
                           )

target_link_libraries(${prjName} PUBLIC  common)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
