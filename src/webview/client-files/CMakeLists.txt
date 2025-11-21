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
	set(NODE_MODULES ${CMAKE_CURRENT_BINARY_DIR}/node_modules)
	set(NODE_MODULES_DEPS client-deps-loaded)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/package.json ${CMAKE_CURRENT_BINARY_DIR}/package.json COPYONLY)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/package-lock.json ${CMAKE_CURRENT_BINARY_DIR}/package-lock.json COPYONLY)
	add_custom_command (OUTPUT client-deps-loaded
					COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../prepare.sh ${CMAKE_CURRENT_BINARY_DIR}
					COMMENT "Fetching webview client javascript dependencies..."
					DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../prepare.sh
					WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	add_custom_target(run-client-deps-loaded ALL DEPENDS client-deps-loaded package.json package-lock.json)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/node_modules DESTINATION share/malt/webview)
else (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/node_modules)
	set(NODE_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/node_modules)
	set(NODE_MODULES_DEPS "")
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/node_modules DESTINATION share/malt/webview)
endif (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/node_modules)

#######################################################
# build
add_custom_command (OUTPUT webview-client-generated
					COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build.sh "${CMAKE_CURRENT_BINARY_DIR}" "${NODE_MODULES}"
					COMMENT "Building client webview..."
					DEPENDS package.json package-lock.json
					WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(webview-client-build ALL DEPENDS webview-client-generated ${NODE_MODULES_DEPS} src package.json package-lock.json vite.config.ts eslint.config.ts env.d.ts)

#######################################################
## Install the webview into share directory
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/ DESTINATION share/malt/webview/dist)
