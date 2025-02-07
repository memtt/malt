############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014 - 2024
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat (INRIA) - 2023 - 2024
############################################################

############################################################
#Usage of OBJECT keyword in add_library require 2.8.8 (or something like that, need to check)
cmake_minimum_required(VERSION 2.8.12)
# TODO we can remove this if/else when we do not use anymore centos7 and move up cmake req to 3.0
if (CMAKE_VERSION VERSION_LESS 3.0)
	project(MALT)	
else()
	cmake_policy(SET CMP0048 NEW)	
	project(MALT VERSION 1.2.4)
endif()

############################################################
#set versions to dispatch in code
set(MALT_VERSION "1.2.4")
set(MALT_VERSION_NOTE "-dev")
set(MALT_SO_VERSION "1")
set(MALT_JSON_FORMAT_VERSION "1.1")

############################################################
#global setup
enable_testing()

############################################################
#Add internal search path for scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

############################################################
# Options
set(ENABLE_TESTS "no" CACHE BOOL "Enable build of internal tests.")
set(ENABLE_GCC_COVERAGE "no" CACHE BOOL "Enable coverage compile flags to be enabled.")

############################################################
#Delegate some cmake scripts
include(${CMAKE_SOURCE_DIR}/cmake/macros.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/unittest-macros.cmake)

############################################################
#check for lib/lib64 suffix
include(GNUInstallDirs)

############################################################
#Some global setup
#malt_enable_cxx_11()
malt_setup_google_tests()
if (ENABLE_GCC_COVERAGE)
	malt_enable_gcc_coverage()
endif(ENABLE_GCC_COVERAGE)

############################################################
set(ENABLE_PROFILER "yes" CACHE BOOL "Enable the installation of the profiler.")

############################################################
# search deps
find_package(GTest QUIET)
FIND_PACKAGE(OpenMP QUIET)
find_package(LibElf QUIET)
find_package(Libunwind QUIET)
find_package(Qt5Widgets QUIET)
find_package(Qt5WebEngineWidgets QUIET)
find_package(Qt5WebKitWidgets QUIET)
find_package(Qt5Network QUIET)
# Check for npm if needed
if (NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/src/webview/node_modules)
	find_package(Npm REQUIRED)
endif()

############################################################
# check for python support
find_package(Python3 REQUIRED COMPONENTS Interpreter Development QUIET)
# we currently support only Python
if (NOT Python3_INTERPRETER_ID STREQUAL "Python")
	message(FATAL_ERROR "Invalid python interpreter, got '${Python3_INTERPRETER_ID}', support only 'Python' !")
endif()

############################################################
#build subdirs
add_subdirectory(src)
if (ENABLE_PROFILER)
	add_subdirectory(extern-deps)
endif(ENABLE_PROFILER)

############################################################
# infos
malt_print_status()
