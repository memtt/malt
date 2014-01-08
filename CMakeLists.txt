######################################################
cmake_minimum_required(VERSION 2.7)
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
