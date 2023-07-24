SET(prjName cppLevel0L)
set(srcS)
file(GLOB srcS src/*.cpp)
set(defS)
set(osSrc)
if (WIN32)
	# add_definitions(-DUNICODE -D_UNICODE)
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W2)
	# ADD_DEFINITIONS(/Od)
	file(GLOB defS src/*.def)
	file(GLOB osSrc src/win/*.cpp)
	#add_definitions(-w)
	#add_definitions(-DWIN32_LEAN_AND_MEAN)
	MESSAGE(STATUS "windows ${prjName} osSrc =  ${osSrc}")
	include_directories(
		${CMAKE_SOURCE_DIR}/../include
		)
	link_directories(
		${CMAKE_SOURCE_DIR}/../lib
		)
endif ()

add_library(${prjName} SHARED ${srcS} ${defS} ${osSrc})
target_include_directories(${prjName} PUBLIC 
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/cLog/src
							src
                           )
					   target_link_libraries(${prjName} PUBLIC  midFrame cLog)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

