SET(prjName appClientServer)
SET(depIncludeHome D:/pzh/work/include/)
SET(depLibHomeD:/pzh/work/lib/)
SET(frameHomeD:/pzh/work/appFrame/)
SET(frameLibPathD:/pzh/work/appFrame/bin/Debug/)
SET(outPutPathD:/pzh/work/appFrame/bin/)

set(srcS)
set(genSrcS)
file(GLOB srcS src/*.cpp)
set(defS)
set(srcOS)
if (UNIX)
    MESSAGE(STATUS "unix")
elseif (WIN32)
	MESSAGE(STATUS "windows")
	file(GLOB defS src/gen/*.def)
	file(GLOB genSrcS src/gen/*.cpp)
	file(GLOB srcOS src/win/*.cpp)
	include_directories(
		${depIncludeHome}
		)
	link_directories(
		${depLibHome}
		)
endif ()
add_library(${prjName} SHARED ${genSrcS} ${srcS} ${srcOS} ${defS})
target_include_directories(${prjName} PUBLIC 
							src
							${frameHome}/common/src
							${frameHome}/testLogicMsg/src
							${frameHome}/logicCommon/src
							${frameHome}/cLog/src
                           )
target_link_libraries(${prjName} PUBLIC  common logicCommon defMsg)
SET(LIBRARY_OUTPUT_PATH ${outPutPath})