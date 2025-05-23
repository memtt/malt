############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindNpm.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2024
#    AUTHOR   : Antoine Bernard (crans.org) - 2024
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
