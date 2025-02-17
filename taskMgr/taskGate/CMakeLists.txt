SET(prjName taskGateModule)
set(serSrcS)
set(genSrcS)
file(GLOB genSrcS src/userLogic/*.cpp src/*.cpp src/taskGateConTh/*.cpp src/taskGateConTh/userLogic/*.cpp src/taskGateNetTh/*.cpp src/taskGateNetTh/userLogic/*.cpp )
set(defS)
set(libPath)
set(libDep)
set(osDepLib)
if (WIN32)
	MESSAGE(STATUS "windows")
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W1)
	file(GLOB defS src/win/*.def)

	include_directories( )
endif ()
	include_directories(
	
	${CMAKE_SOURCE_DIR}/gen/taskMgrConfig/src
src/userLogic
C:/work/appFrameInstall/include/appFrame
${CMAKE_SOURCE_DIR}/gen/taskGate/src
${CMAKE_SOURCE_DIR}/gen/taskGate/src/taskGateConTh
src/taskGateConTh
src/taskGateConTh/userLogic
${CMAKE_SOURCE_DIR}/gen/taskGate/src/taskGateNetTh
src/taskGateNetTh
src/taskGateNetTh/userLogic
)
list(APPEND libPath C:/work/appFrameInstall/lib)
link_directories(${libPath} ${libDep})
	add_library(${prjName} SHARED ${genSrcS} ${defS})
if (UNIX)
	MESSAGE(STATUS "unix add -fPIC")
	find_package(unofficial-libuuid CONFIG REQUIRED)
	target_compile_options(${prjName} PRIVATE -fPIC)	
    set(VERSION_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/src/unix/lib.vers")
	target_link_options(${prjName} PRIVATE -Wl,-version-script,${VERSION_SCRIPT})
	list(APPEND osDepLib unofficial::UUID::uuid)
endif ()

target_link_libraries(${prjName} PRIVATE
	common
	taskMgrConfig
	taskGateLib 
	logicCommon
	frameConfig
	cLog
	comTcpProNet
	${osDepLib}

	)
	SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
	install(TARGETS ${prjName} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
	