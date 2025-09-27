cmake_minimum_required(VERSION 3.16) 
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_USE_64BITS on)
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 

set (myProjectName appFrame CACHE INTERNAL "Shared variable")
project(${myProjectName})
SET(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/../${myProjectName}Install)

# set (firstBuildInc ${CMAKE_INSTALL_PREFIX}/include/appFrame  CACHE INTERNAL "Shared variable")
# set (firstBuildLib ${CMAKE_INSTALL_PREFIX}/lib  CACHE INTERNAL "Shared variable")
# set (firstBuildDir ${CMAKE_SOURCE_DIR}../firstBuildInstall CACHE INTERNAL "Shared variable")

# 1. 首先，定义一个默认值（优先级最低）
set(firstBuildDirDef ${CMAKE_SOURCE_DIR}/../firstBuildInstall)

# 2. 其次，检查环境变量是否存在（优先级中）
if(DEFINED ENV{firstBuildDir})
    set(firstBuildDirDef "$ENV{firstBuildDir}")
endif()

# 3. 最后，创建缓存变量。如果命令行通过 -D 传递了值，它将覆盖上述默认值（优先级最高）
set(firstBuildDir "${firstBuildDirDef}" CACHE PATH "Path of firstBuild install")

# 使用变量
message(STATUS "最终firstBuildDir是: ${firstBuildDir}")

add_subdirectory (cppLevel0L)
# add_subdirectory (cppLevel0)
add_subdirectory (logicCommon)
add_subdirectory (libeventSession)
add_subdirectory (frameConfig)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
