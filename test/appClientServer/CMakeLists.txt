SET(prjName appClientServer)
set(genSrcS)
file(GLOB genSrcS src/gen/*.cpp)
set(defS)
set(libPath)
set(libDep)
if (WIN32)
	MESSAGE(STATUS "windows")
	file(GLOB defS src/gen/win/*.def)
	include_directories(D:/pzh/work/include/)
list(APPEND libDep D:/pzh/work/lib/)
endif ()
add_library(${prjName} SHARED ${genSrcS} ${defS})
	include_directories(
    D:/pzh/work/appFrame/common/src
    D:/pzh/work/appFrame/logicCommon/src
    D:/pzh/work/appFrame/cLog/src
)
list(APPEND libPath D:/pzh/work/appFrame/bin/Debug/)
link_directories(${libPath} ${libDep})
if (UNIX)
	target_link_libraries(${prjName} PUBLIC
		common
		logicCommon
		cLog
		)
elseif (WIN32)
	target_link_libraries(${prjName} PUBLIC
D:/pzh/work/appFrame/bin/Debug/common.lib
D:/pzh/work/appFrame/bin/Debug/logicCommon.lib
D:/pzh/work/appFrame/bin/Debug/cLog.lib
)
endif ()
SET(LIBRARY_OUTPUT_PATH D:/pzh/work/appFrame/bin/)