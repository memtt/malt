######################################################
#Usage of OBJECT keyword in add_library require 2.8.3 (or something like that, need to check)
cmake_minimum_required(VERSION 2.8.3)
project(callstacktrace)

######################################################
#global setup
enable_testing()

######################################################
#Add internal search path for scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

######################################################
#Delegate some cmake scripts
include(${CMAKE_SOURCE_DIR}/cmake/macros.cmake)

######################################################
#Some global setup
setup_internal_gmock_and_gtest()

######################################################
#build subdirs
add_subdirectory(src)
add_subdirectory(extern-deps)