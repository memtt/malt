############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindCargo.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
# - Try to find cargo command for Rust building.
# Once done this will define
#  CAGO_FOUND - System has cargo
#  CAGO_BINARY - Path to the cargo binary file

############################################################
set(CARGO_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE STRING "Help cmake to find Cargo binary (https://doc.rust-lang.org/cargo/index.html) into your system.")

############################################################
find_program(CARGO_BINARY cargo
	HINTS ${CARGO_PREFIX}/bin)

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set NPM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Cargo "Cannot find cargo. disabling the rust-based new webview."
	CARGO_BINARY)

############################################################
mark_as_advanced(CARGO_BINARY)
