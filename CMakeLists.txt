######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.0                        #
#            DATE     : 03/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#Usage of OBJECT keyword in add_library require 2.8.8 (or something like that, need to check)
cmake_minimum_required(VERSION 2.8.8)
project(MALT)

######################################################
#set versions to dispatch in code
set(MALT_VERSION "1.2.0")
set(MALT_VERSION_NOTE "-dev")
set(MALT_SO_VERSION "1")
set(MALT_JSON_FORMAT_VERSION "1.1")

######################################################
#global setup
enable_testing()

######################################################
#Add internal search path for scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

######################################################
#Delegate some cmake scripts
include(${CMAKE_SOURCE_DIR}/cmake/macros.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/unittest-macros.cmake)

######################################################
#check for lib/lib64 suffix
include(GNUInstallDirs)

######################################################
#Some global setup
#malt_enable_cxx_11()
setup_internal_gmock_and_gtest()
if (ENABLE_GCC_COVERAGE)
	malt_enable_gcc_coverage()
endif(ENABLE_GCC_COVERAGE)

######################################################
#build subdirs
add_subdirectory(src)
add_subdirectory(extern-deps)
