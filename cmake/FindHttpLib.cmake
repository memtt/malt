############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 09/2025
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindHttpLib.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
# - Try to find httplib (https://github.com/yhirose/cpp-httplib)
# Once done this will define
#  HTTPLIB_FOUND - System has httplib
#  HTTPLIB_INCLUDE_DIRS - The httplib include directories
#  HTTPLIB_LIBRARIES - The libraries needed to use httplib
#  HTTPLIB_DEFINITIONS - Compiler switches required for using httplib

############################################################
set(HTTPLIB_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE STRING "Help cmake to find httplib library (https://github.com/yhirose/cpp-httplib) into your system.")

############################################################
include(GNUInstallDirs)

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
