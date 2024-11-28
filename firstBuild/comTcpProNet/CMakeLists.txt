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


if (WIN32)
	add_library(${prjName} ${srcS})
elseif (UNIX)
	add_library(${prjName} ${srcS})
	#add_library(${prjName} SHARED ${srcS})
	#target_compile_options(${prjName} PRIVATE -fPIC)
	#target_compile_options(${prjName} PRIVATE --verbose)
	#target_link_options(${prjName} PRIVATE -Wl,-version-script,${VERSION_SCRIPT})
	#target_link_options(${prjName} PRIVATE --verbose)
	#target_link_libraries(${prjName} PRIVATE common)
	#set_target_properties(${prjName} PROPERTIES		COMPILE_FLAGS "-fvisibility=hidden")

endif ()


target_include_directories(${prjName} PRIVATE 
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/cLog/src
                           )

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

install(TARGETS ${prjName} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

set(incS src/comTcpNet.h)
install(FILES ${incS} DESTINATION include/${myProjectName})
