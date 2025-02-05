############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindLibunwind.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2020 - 2024
############################################################

############################################################
# - Try to find libunwind (http://directory.fsf.org/wiki/Libelf)
# Once done this will define
#  LIBUNWIND_FOUND - System has libunwind
#  LIBUNWIND_INCLUDE_DIRS - The libunwind include directories
#  LIBUNWIND_LIBRARIES - The libraries needed to use libunwind
#  LIBUNWIND_DEFINITIONS - Compiler switches required for using libunwind

############################################################
set(LIBUNWIND_PREFIX "" CACHE STRING "Help cmake to find libunwind library (http://www.nongnu.org/libunwind/) into your system.")

############################################################
find_path(LIBUNWIND_INCLUDE_DIR libunwind.h
	HINTS ${LIBUNWIND_PREFIX}/include)

############################################################
find_library(LIBUNWIND_LIBRARY NAMES unwind
	HINTS ${LIBUNWIND_PREFIX}/${LIBDIR})

############################################################
set(arch ${CMAKE_SYSTEM_PROCESSOR})
find_library(LIBUNWIND_${arch} NAMES unwind-${arch}
	HINTS ${LIBUNWIND_PREFIX}/${LIBDIR})
if (LIBUNWIND_${arch})
	list(APPEND LIBUNWIND_ARCH_LIBRARIES ${LIBUNWIND_${arch}})
endif()

############################################################
set(LIBUNWIND_LIBRARIES ${LIBUNWIND_LIBRARY} ${LIBUNWIND_ARCH_LIBRARIES})
set(LIBUNWIND_INCLUDE_DIRS ${LIBUNWIND_INCLUDE_DIR} )

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBUNWIND_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Libunwind  DEFAULT_MSG
	LIBUNWIND_LIBRARY LIBUNWIND_LIBRARIES LIBUNWIND_INCLUDE_DIR)

############################################################
mark_as_advanced(LIBUNWIND_INCLUDE_DIR LIBUNWIND_LIBRARY )
