SET(prjName logicCommon)
set(srcS)
set(osDepsrcS) 
set(libDir ${firstBuildLib})
file(GLOB srcS src/*.cpp)

set(libS comTcpProNet)
if (UNIX)
	add_compile_options(-fPIC)
	find_package(unofficial-libuuid CONFIG REQUIRED)
	file(GLOB osDepsrcS src/unix/*.cpp)
elseif (WIN32)
    MESSAGE(STATUS "windows")
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-w)
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W1)
	file(GLOB osDepsrcS src/win/*.cpp)
endif ()

link_directories(
		${libDir}
		)

if (WIN32)
	add_library(${prjName} ${srcS} ${osDepsrcS})
elseif (UNIX)
	#add_library(${prjName} SHARED ${srcS})
	add_library(${prjName} ${srcS} ${osDepsrcS})
endif ()


target_include_directories(${prjName} PRIVATE 
							${CMAKE_SOURCE_DIR}/frameConfig/src
							${firstBuildInc} 
                           )

# target_link_libraries(${prjName} PRIVATE  ${libS})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

install(TARGETS ${prjName} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

set(incS src/gLog.h  src/logicFrameConfig.h src/logicWorker.h src/routeWorker.h src/internalRpc.h src/internalChannel.h src/SingleConnectorWorker.h src/gateRouteWorker.h src/gateCommonWorker.h src/serverRouteWorker.h src/serverCommonWorker.h src/logicWorkerMgr.h)

install(FILES ${incS} DESTINATION include/${myProjectName})
