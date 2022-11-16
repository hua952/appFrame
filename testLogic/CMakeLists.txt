SET(prjName testLogic)
set(srcS)
file(GLOB srcS src/*.cpp)
set(defS)
if (WIN32)
    MESSAGE(STATUS "windows")
	file(GLOB defS src/*.def)
endif ()


add_library(${prjName} SHARED ${srcS} ${defS})

target_include_directories(${prjName} PUBLIC 
							${CMAKE_SOURCE_DIR}/common/src
							${CMAKE_SOURCE_DIR}/testLogicMsg/src
							${CMAKE_SOURCE_DIR}/logicCommon/src
                           )
target_link_libraries(${prjName} PUBLIC  common)
target_link_libraries(${prjName} PUBLIC  testLogicMsg)
target_link_libraries(${prjName} PUBLIC logicCommon)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
