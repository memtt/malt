############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 03/2025
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindMaltJeMalloc.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
# - Try to find JeMalloc built for MALT (https://github.com/jemalloc/jemalloc)
# - It SHOULD be built with options :
# - ../configure --with-jemalloc-prefix=malt_je_ --prefix=$HOME/usr-jemalloc --with-private-namespace=malt_je_ --with-install-suffix=-malt --enable-shared
# Once done this will define
#  MALTJEMALLOC_FOUND - System has libunwind
#  MALTJEMALLOC_INCLUDE_DIRS - The libunwind include directories
#  MALTJEMALLOC_LIBRARIES - The libraries needed to use libunwind
#  MALTJEMALLOC_DEFINITIONS - Compiler switches required for using libunwind

############################################################
set(MALTJEMALLOC_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE STRING "Help cmake to find MALT build of JeMalloc library (https://github.com/jemalloc/jemalloc) into your system. Note that it should be configured with optione : './configure --with-jemalloc-prefix=malt_je_ --with-private-namespace=malt_je_ --with-install-suffix=-malt --enable-shared'")

############################################################
find_path(MALTJEMALLOC_INCLUDE_DIR jemalloc-malt.h
	HINTS ${MALTJEMALLOC_PREFIX}/include/jemalloc)

############################################################
find_library(MALTJEMALLOC_LIBRARY NAMES jemalloc-malt_pic
	HINTS ${MALTJEMALLOC_PREFIX}/lib
	      ${MALTJEMALLOC_PREFIX}/${LIBDIR})

############################################################
set(MALTJEMALLOC_LIBRARIES ${MALTJEMALLOC_LIBRARY})
set(MALTJEMALLOC_INCLUDE_DIRS ${MALTJEMALLOC_INCLUDE_DIR} )

############################################################
if (MALTJEMALLOC_LIBRARIES AND MALTJEMALLOC_INCLUDE_DIRS)
	add_library(malt_je_malloc::jemalloc INTERFACE IMPORTED)
	target_link_libraries(malt_je_malloc::jemalloc INTERFACE ${MALTJEMALLOC_LIBRARY})
	target_include_directories(malt_je_malloc::jemalloc INTERFACE ${MALTJEMALLOC_INCLUDE_DIR})
endif()

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set MALTJEMALLOC_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(MaltJeMalloc  DEFAULT_MSG
	MALTJEMALLOC_LIBRARY MALTJEMALLOC_LIBRARIES MALTJEMALLOC_INCLUDE_DIR)

############################################################
mark_as_advanced(MALTJEMALLOC_INCLUDE_DIR MALTJEMALLOC_LIBRARY)
