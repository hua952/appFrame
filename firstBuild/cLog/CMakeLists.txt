SET(prjName cLog)
set(srcS)
file(GLOB srcS src/*.cpp)
set(defS)
if (UNIX)
	add_compile_options(-fPIC)
elseif (WIN32)
    MESSAGE(STATUS "windows")
	add_definitions(-w)
	include_directories(
							${CMAKE_SOURCE_DIR}/../include
							$ENV{depInclude}
							)
	link_directories(
		${CMAKE_SOURCE_DIR}/../lib
		)
endif ()

add_library(${prjName} ${srcS} )

target_include_directories(${prjName} PRIVATE 
							${CMAKE_SOURCE_DIR}/common/src
                           )
					   # target_link_libraries(${prjName} PRIVATE  common)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

install(TARGETS ${prjName} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
set(incS src/cLog.h)
install(FILES ${incS} DESTINATION include/${myProjectName})
