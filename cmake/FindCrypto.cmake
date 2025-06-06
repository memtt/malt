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
#  CRYPTO_FOUND - System has httplib
#  CRYPTO_INCLUDE_DIRS - The httplib include directories
#  CRYPTO_LIBRARIES - The libraries needed to use httplib
#  CRYPTO_DEFINITIONS - Compiler switches required for using httplib

############################################################
set(CRYPTO_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE STRING "Help cmake to find httplib library (https://zeromq.org/) into your system.")

############################################################
find_path(CRYPTO_INCLUDE_DIR openssl/sha.h
	HINTS ${CRYPTO_PREFIX}/include)

############################################################
find_library(CRYPTO_LIBRARY NAMES crypto
	HINTS ${CRYPTO_PREFIX}/${LIBDIR})

############################################################
set(CRYPTO_LIBRARIES ${CRYPTO_LIBRARY} )
set(CRYPTO_INCLUDE_DIRS ${CRYPTO_INCLUDE_DIR} )

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CRYPTO_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Crypto  DEFAULT_MSG
	CRYPTO_LIBRARY CRYPTO_INCLUDE_DIR)

############################################################
mark_as_advanced(CRYPTO_INCLUDE_DIR CRYPTO_LIBRARY )
