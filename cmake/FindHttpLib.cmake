############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.6
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindLibElf.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2022 - 2024
############################################################

############################################################
# - Try to find httplib (https://github.com/yhirose/cpp-httplib)
# Once done this will define
#  HTTPLIB_FOUND - System has httplib
#  HTTPLIB_INCLUDE_DIRS - The httplib include directories
#  HTTPLIB_LIBRARIES - The libraries needed to use httplib
#  HTTPLIB_DEFINITIONS - Compiler switches required for using httplib

############################################################
set(HTTPLIB_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE STRING "Help cmake to find httplib library (https://zeromq.org/) into your system.")

############################################################
find_path(HTTPLIB_INCLUDE_DIR httplib.h
	HINTS ${HTTPLIB_PREFIX}/include)

############################################################
find_library(HTTPLIB_LIBRARY NAMES cpp-httplib
	HINTS ${HTTPLIB_PREFIX}/${LIBDIR})

############################################################
set(HTTPLIB_LIBRARIES ${HTTPLIB_LIBRARY} )
set(HTTPLIB_INCLUDE_DIRS ${HTTPLIB_INCLUDE_DIR} )

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set HTTPLIB_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(HttpLib  DEFAULT_MSG
	HTTPLIB_LIBRARY HTTPLIB_INCLUDE_DIR)

############################################################
mark_as_advanced(HTTPLIB_INCLUDE_DIR HTTPLIB_LIBRARY )
