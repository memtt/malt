############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 09/2025
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindBinutils.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
# - Try to find nm command from binutils.
# Once done this will define
#  BINUTILS_FOUND - System has nm
#  BINUTILS_BINARY - Path to the nm binary file

############################################################
set(BINUTILS_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE STRING "Help cmake to find nm & addr2line binaries from binutils (https://www.gnu.org/software/binutils/) into your system.")

############################################################
find_program(NM_BINARY nm
	HINTS ${BINUTILS_PREFIX}/bin)

############################################################
find_program(ADDR2LINE_BINARY addr2line
	HINTS ${BINUTILS_PREFIX}/bin)

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set NPM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Binutils "Cannot find binutils (nm & addr2line)."
	NM_BINARY
	ADDR2LINE_BINARY)

############################################################
mark_as_advanced(NM_BINARY ADDR2LINE_BINARY)
