############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.0
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindNode.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
# - Try to find node command
# Once done this will define
#  NODE_FOUND - System has npm
#  NODE_BINARY - Path to the npm binary file

############################################################
set(NODEJS_PATH ${CMAKE_INSTALL_PREFIX} CACHE STRING "Prefix where to search NodeJs command (node).")

############################################################
find_program(NODE_BINARY node
    HINTS ${NODEJS_PATH}/bin)

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set NODE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Node  "Cannot find NODE."
	NODE_BINARY)

############################################################
mark_as_advanced(NODE_BINARY)
