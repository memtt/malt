############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.6
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindNpm.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2025
############################################################

############################################################
# - Try to find cargo command for Rust building.
# Once done this will define
#  CAGO_FOUND - System has cargo
#  CAGO_BINARY - Path to the cargo binary file

############################################################
find_program(CARGO_BINARY cargo)

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set NPM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Cargo "Cannot find cargo. disabling the rust-based new webview."
	CARGO_BINARY)

############################################################
mark_as_advanced(CARGO_BINARY)
