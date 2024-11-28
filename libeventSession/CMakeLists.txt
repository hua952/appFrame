SET(prjName libeventSession)
set(srcS)
set(libDir ${firstBuildLib})
file(GLOB srcS src/*.cpp)
set(defS)
set(srcH)
set(osSrc)

find_package(Libevent CONFIG  REQUIRED
	# PATHS $ENV{VCPKG_HOME}/packages/libevent_$ENV{VCPKG_STATIC}/share/libevent
	# NO_DEFAULT_PATH
)
	
if (UNIX)
	set(srcH src/unix)
	file(GLOB osSrc src/unix/*.cpp)
	add_compile_options(-fPIC)
elseif (WIN32)
    MESSAGE(STATUS "windows")
	set(srcH src/win)
	file(GLOB osSrc src/win/*.cpp)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-w)
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W2)
	include_directories(
							${CMAKE_SOURCE_DIR}/../include
							# $ENV{depInclude}
							)
	# list(APPEND libDir	$ENV{depLib})
	file(GLOB defS src/*.def)
endif ()

link_directories(${libDir})

add_library(${prjName} SHARED ${srcS} ${osSrc} ${defS})

target_include_directories(${prjName} PRIVATE 
							${srcH}
							${firstBuildInc} 
                           )
target_link_libraries(${prjName} PRIVATE  common
	PRIVATE libevent::core libevent::extra
	)

if (WIN32)
	target_link_libraries(${prjName} PRIVATE ws2_32
	)
elseif (UNIX)
    set(VERSION_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/lib.vers")
	target_compile_options(${prjName} PRIVATE -fPIC)
	target_link_options(${prjName} PRIVATE -Wl,-version-script,${VERSION_SCRIPT})
endif ()

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

