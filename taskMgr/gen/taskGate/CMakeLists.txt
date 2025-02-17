SET(prjName taskGateLib)
set(serSrcS)
set(genSrcS)
file(GLOB genSrcS src/*.cpp src/taskGateConTh/*.cpp src/taskGateNetTh/*.cpp )
file(GLOB mainSrcS src/main.cpp )
set(defS)
set(libPath)
set(libDep)
set(mainLibS)
set(osSrc)
if (UNIX)
    MESSAGE(STATUS "unix")
	file(GLOB osSrc src/unix/*.cpp)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
elseif (WIN32)
	MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W1)
	file(GLOB osSrc src/win/*.cpp)
	list(APPEND mainLibS ws2_32)
	include_directories( )
endif ()
	include_directories(
	src
	${CMAKE_SOURCE_DIR}/gen/taskMgrConfig/src
src/userLogic
C:/work/appFrameInstall/include/appFrame
src/taskGateConTh
src/taskGateNetTh
)
list(APPEND libPath C:/work/appFrameInstall/lib)
link_directories(${libPath} ${libDep})
	add_library(${prjName} ${genSrcS})
	add_executable (taskGate ${mainSrcS} ${osSrc})
target_link_libraries(taskGate PRIVATE
	common
	${mainLibS}
	)

	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
	install(TARGETS taskGate RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
	