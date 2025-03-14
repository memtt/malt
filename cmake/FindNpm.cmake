############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.6
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : cmake/FindNpm.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2024
#    AUTHOR   : Antoine Bernard (crans.org) - 2024
############################################################

############################################################
# - Try to find npm command
# Once done this will define
#  NPM_FOUND - System has npm
#  NPM_BINARY - Path to the npm binary file

############################################################
find_program(NPM_BINARY npm)

############################################################
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set NPM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Npm  "Cannot find NPM. This is required beause you took the git master branch. Download official tar.gz or disable webview."
	NPM_BINARY)

############################################################
mark_as_advanced(NPM_BINARY)
