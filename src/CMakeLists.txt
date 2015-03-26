######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.1.0                        #
#            DATE     : 03/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_subdirectory(lib)
add_subdirectory(integration)
add_subdirectory(trace-reader)

######################################################
check_nodejs()

######################################################
# Install the webview into share directory
INSTALL(DIRECTORY webview DESTINATION share/malt
	PATTERN webview/client_v2/node_modules EXCLUDE)
