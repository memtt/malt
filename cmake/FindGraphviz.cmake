############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindGraphviz.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
# - Try to find dot command for webview graph rendering.
# Once done this will define
#  GRAPHVIZ_FOUND - System has graphviz
#  GRAPHVIZ_BINARY - Path to the graphviz binary file

############################################################
set(GRAPHVIZ_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE STRING "Help cmake to find Graphviz dot binary (https://graphviz.org/) into your system.")

############################################################
find_program(DOT_BINARY dot
	HINTS ${GRAPHVIZ_PREFIX}/bin)

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set NPM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Graphviz "Cannot find graphviz. disabling some webview features."
	DOT_BINARY)

############################################################
mark_as_advanced(DOT_BINARY)
