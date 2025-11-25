############################################################
#    PROJECT  : MALT (MALoc Tracker)
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
					WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	add_custom_target(run-deps-loaded ALL DEPENDS deps-loaded package.json package-lock.json)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/node_modules DESTINATION share/malt/webview)
else (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/node_modules)
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/node_modules DESTINATION share/malt/webview)
endif (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/node_modules)

#######################################################
## Install the webview into share directory
install(FILES ./malt-webserver.js DESTINATION share/malt/webview)
install(DIRECTORY ./server-files DESTINATION share/malt/webview)

#######################################################
add_subdirectory(client-files)