######################################################
# - Try to find libunwind (http://directory.fsf.org/wiki/Libelf)
# Once done this will define
#  LIBUNWIND_FOUND - System has libunwind
#  LIBUNWIND_INCLUDE_DIRS - The libunwind include directories
#  LIBUNWIND_LIBRARIES - The libraries needed to use libunwind
#  LIBUNWIND_DEFINITIONS - Compiler switches required for using libunwind

######################################################
set(LIBUNWIND_PREFIX "" CACHE STRING "Help cmake to find libunwind library (http://www.nongnu.org/libunwind/) into your system.")

######################################################
find_path(LIBUNWIND_INCLUDE_DIR libunwind.h
	HINTS ${LIBUNWIND_PREFIX}/include)

######################################################
find_library(LIBUNWIND_LIBRARY NAMES unwind
	HINTS ${LIBUNWIND_PREFIX}/lib)

######################################################
set(LIBUNWIND_LIBRARIES ${LIBUNWIND_LIBRARY} )
set(LIBUNWIND_INCLUDE_DIRS ${LIBUNWIND_INCLUDE_DIR} )

######################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBUNWIND_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(libunwind  DEFAULT_MSG
	LIBUNWIND_LIBRARY LIBUNWIND_INCLUDE_DIR)

######################################################
mark_as_advanced(LIBUNWIND_INCLUDE_DIR LIBUNWIND_LIBRARY )
