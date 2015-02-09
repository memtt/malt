######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.0.0                        #
#            DATE     : 02/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_subdirectory(lib)
add_subdirectory(integration)
#add_subdirectory(trace-reader)

######################################################
# Install the webview into share directory
INSTALL(DIRECTORY webview DESTINATION share/matt)
