############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 09/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/webview/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Antoine Bernard (crans.org) - 2024
############################################################

######################################################
# Install nodejs dependencies if needed
# then install node_modules into share directory
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/node_modules)
	message(STATUS "Copying package.json and package-lock.json to build directory")
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/package.json ${CMAKE_CURRENT_BINARY_DIR}/package.json COPYONLY)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/package-lock.json ${CMAKE_CURRENT_BINARY_DIR}/package-lock.json COPYONLY)
	add_custom_command (OUTPUT deps-loaded
					COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/prepare.sh ${CMAKE_CURRENT_BINARY_DIR}
					COMMENT "Fetching webview javascript dependencies..."
					DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/prepare.sh
					WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	add_custom_target(run ALL DEPENDS deps-loaded package.json package-lock.json)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/node_modules DESTINATION share/malt/webview)
else (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/node_modules)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/node_modules DESTINATION share/malt/webview)
endif (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/node_modules)

#######################################################
## Install the webview into share directory
INSTALL(DIRECTORY . DESTINATION share/malt/webview)
