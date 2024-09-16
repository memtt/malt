######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 06/2023                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
# Use npm to install project dependencies
message(STATUS "Installing project dependencies using npm...")
execute_process(
    COMMAND npm ci
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/webview
    RESULT_VARIABLE npm_cleaninstall_result
)

if(NOT npm_cleaninstall_result EQUAL 0)
    message(FATAL_ERROR "Failed to install dependencies via npm.")
else()
    message(STATUS "Dependencies installed successfully with npm.")
endif()

######################################################
# Install the webview into share directory
INSTALL(DIRECTORY . DESTINATION share/malt/webview
	PATTERN client_v2/node_modules EXCLUDE)
