cmake_minimum_required (VERSION 3.16)

SET(prjName showStack)
#project (myproj VERSION 1.0)

#find_package (glog 0.6.0 REQUIRED)

add_executable (${prjName} src/main.cpp)

target_link_libraries(${prjName} PUBLIC dbghelp)

