############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 09/2025
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindCrypto.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
# - Try to find OPENSSL (https://www.openssl-library.org/source/index.html)
# Once done this will define
#  CRYPTO_FOUND - System has openssl
#  CRYPTO_INCLUDE_DIRS - The openssl include directories
#  CRYPTO_LIBRARIES - The libraries needed to use openssl
#  CRYPTO_DEFINITIONS - Compiler switches required for using openssl

############################################################
set(CRYPTO_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE STRING "Help cmake to find openssl library (https://www.openssl-library.org/source/index.html) into your system.")

############################################################
include(GNUInstallDirs)

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
find_package_handle_standard_args(Crypto DEFAULT_MSG
	CRYPTO_LIBRARY CRYPTO_INCLUDE_DIR)

############################################################
mark_as_advanced(CRYPTO_INCLUDE_DIR CRYPTO_LIBRARY )
