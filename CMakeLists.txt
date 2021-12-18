cmake_minimum_required(VERSION 3.16) 
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 
# set the project name 
project(mainLoop) 
add_library(mainLoop SHARED src/mainLoop.cpp)
target_include_directories(mainLoop PUBLIC 
                           "include" 
                           ) 
