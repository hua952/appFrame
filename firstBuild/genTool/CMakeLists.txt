SET(prjName genTool)
# set(VCPKG_TARGET_TRIPLET "x64-windows-static")
# set(fmt_DIR "$ENV{VCPKG_HOME}/installed/x64-windows-static/share/fmt")
find_path(RAPIDXML_INCLUDE_DIRS "rapidxml/rapidxml.hpp")
MESSAGE(STATUS "RAPIDXML_INCLUDE_DIRS : ${RAPIDXML_INCLUDE_DIRS} ")
#  target_include_directories(main PRIVATE ${RAPIDXML_INCLUDE_DIRS})
set(srcS)
file(GLOB srcS src/*.cpp src/fromFileData/*.cpp src/procGen/*.cpp)
find_package(fmt CONFIG REQUIRED)
set(osSrc)
if (UNIX)
    MESSAGE(STATUS "unix")
	add_definitions(-w)
	file(GLOB osSrc /src/unix/*.cpp)
elseif (WIN32)
    MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/utf-8)
	ADD_DEFINITIONS(/W1)
	# file(GLOB osSrc /src/win/*.cpp)
	aux_source_directory(src/win osSrc)
	# include_directories($ENV{depInclude})
endif ()

include_directories(
							${CMAKE_SOURCE_DIR}/../include
							)

add_executable (${prjName} ${srcS} ${osSrc})
target_include_directories(${prjName} PRIVATE 
							src
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/cLog/src
							PRIVATE ${RAPIDXML_INCLUDE_DIRS}
							PRIVATE fmt::fmt-header-only
                           )
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
target_link_libraries(${prjName} PRIVATE  common cLog)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS ${prjName} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

