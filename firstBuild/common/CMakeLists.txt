SET(prjName common)
set(CMAKE_CXX_STANDARD 20) 
set(srcS)
file(GLOB srcS src/*.cpp)
set(osSrc)

# set(libS)
MESSAGE(STATUS "to make common")
if (UNIX)
    MESSAGE(STATUS "unix")
	#add_definitions(-w)
	file(GLOB osSrc src/unix/*.cpp)
	add_compile_options(-fPIC)
elseif (WIN32)
	file(GLOB osSrc src/win/*.cpp)
	ADD_DEFINITIONS(/Zi)
	ADD_DEFINITIONS(/W2)
	MESSAGE(STATUS "windows osSrc = ${osSrc}")
endif ()

set(incS src/msg.h src/argMgr.h src/arrayMap.h src/comFun.h src/cTimer.h src/depOSFun.h
	src/fpsCount.h src/ITcpServer.h src/ISession.h src/loop.h
	src/mainLoop.h src/modelLoder.h src/msgMgr.h src/myAssert.h src/packet.h
	src/staticArrayMap.h src/strFun.h src/TAlgorithm.h src/tJumpListNS.h
	src/tJumpListNSAuto.h src/tSingleton.h src/typeDef.h)



if (WIN32)
	list(APPEND libS Dbghelp)
	add_library(${prjName} ${srcS} ${osSrc})
elseif (UNIX)
	#add_library(${prjName} ${srcS} ${osSrc})
	add_library(${prjName} SHARED ${srcS} ${osSrc})
	target_compile_options(${prjName} PRIVATE -fPIC)
endif ()

target_include_directories(${prjName} PRIVATE 
							src
                           )
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

install(TARGETS ${prjName}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)
install(FILES ${incS} DESTINATION include/${myProjectName})
