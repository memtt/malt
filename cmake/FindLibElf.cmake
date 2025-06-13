############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.0
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindLibElf.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2022 - 2024
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
# - Try to find libelf (http://directory.fsf.org/wiki/Libelf)
# Once done this will define
#  LIBELF_FOUND - System has libelf
#  LIBELF_INCLUDE_DIRS - The libelf include directories
#  LIBELF_LIBRARIES - The libraries needed to use libelf
#  LIBELF_DEFINITIONS - Compiler switches required for using libelf

############################################################
set(LIBELF_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE STRING "Help cmake to find libelf library (http://directory.fsf.org/wiki/Libelf) into your system.")

############################################################
find_path(LIBELF_INCLUDE_DIR libelf.h elf.h
	HINTS ${LIBELF_PREFIX}/include)

############################################################
find_library(LIBELF_LIBRARY NAMES elf
	HINTS ${LIBELF_PREFIX}/${LIBDIR})

############################################################
set(LIBELF_LIBRARIES ${LIBELF_LIBRARY} )
set(LIBELF_INCLUDE_DIRS ${LIBELF_INCLUDE_DIR} )

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBELF_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibElf  DEFAULT_MSG
	LIBELF_LIBRARY LIBELF_INCLUDE_DIR)

############################################################
mark_as_advanced(LIBELF_INCLUDE_DIR LIBELF_LIBRARY )
