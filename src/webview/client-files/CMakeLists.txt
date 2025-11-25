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
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/extern-deps/node_modules)
	message(STATUS "Copying package.json and package-lock.json to build directory")
	set(NODE_MODULES ${CMAKE_CURRENT_BINARY_DIR}/node_modules)
	set(NODE_MODULES_DEPS deps-loaded)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/package.json ${CMAKE_CURRENT_BINARY_DIR}/package.json COPYONLY)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/package-lock.json ${CMAKE_CURRENT_BINARY_DIR}/package-lock.json COPYONLY)
	add_custom_command (OUTPUT deps-loaded
					COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../prepare.sh ${CMAKE_CURRENT_BINARY_DIR}
					COMMENT "Fetching webview javascript dependencies..."
					DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../prepare.sh
					WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	add_custom_target(run-client-deps-loaded ALL DEPENDS deps-loaded package.json package-lock.json)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/node_modules DESTINATION share/malt/webview)
else ()
	set(NODE_MODULES ${CMAKE_SOURCE_DIR}/extern-deps/node_modules)
	set(NODE_MODULES_DEPS "")
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/node_modules DESTINATION share/malt/webview)
endif ()

#######################################################
# build
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/dist/index.html OR ${CMAKE_CURRENT_SOURCE_DIR}/src IS_NEWER_THAN ${CMAKE_CURRENT_SOURCE_DIR}/dist/index.html)
	message(STATUS "Need building webview-client")
	if (NODE_FOUND)
		file(GLOB_RECURSE WEBVIEW_SRCS LIST_DIRECTORIES false *.css *.scss *.vue *.ts)
		list(APPEND ${WEBVIEW_SRCS} package.json package-lock.json vite.config.ts eslint.config.ts env.d.ts)
		add_custom_command(
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dist/index.html
			COMMENT "Building client webview..."
			BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/dist/assets/index.js
					${CMAKE_CURRENT_BINARY_DIR}/dist/index.html
					${CMAKE_CURRENT_BINARY_DIR}/dist/assets/style.css
			COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build.sh "${CMAKE_CURRENT_BINARY_DIR}" "${NODE_MODULES}"
			DEPENDS ${NODE_MODULES_DEPS} ${WEBVIEW_SRCS}
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
		set(CLIENT_DIST_DIR ${CMAKE_CURRENT_BINARY_DIR}/dist/index.html)
	else()
		message(FATAL_ERROR "Webview client code not pre-built and not have NodeJS to build it !")
	endif()
else()
	message(STATUS "Using pre-built webview-client")
	set(CLIENT_DIST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dist)
endif()

#######################################################
add_custom_target(webview-client-all
	ALL
	DEPENDS ${NODE_MODULES_DEPS} ${CLIENT_DIST_DIR}
)
	
#######################################################
## Install the webview into share directory
install(DIRECTORY ${CLIENT_DIST_DIR} DESTINATION share/malt/webview/dist)
