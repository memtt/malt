######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
check_nodejs()

######################################################
# Find the QtWidgets library
find_package(Qt5Widgets QUIET)
find_package(Qt5WebEngineWidgets QUIET)
find_package(Qt5WebKitWidgets QUIET)
find_package(Qt5Network QUIET)

######################################################
if (Qt5WebEngineWidgets_FOUND OR Qt5WebKitWidgets_FOUND)
	set(Qt5Webkit_FOUND OK)
endif (Qt5WebEngineWidgets_FOUND OR Qt5WebKitWidgets_FOUND)

if (Qt5Widgets_FOUND AND Qt5Webkit_FOUND AND Qt5Network_FOUND)
	set(QT5_FOUND OK)
	message(STATUS "Find QT5 to build view")
else (Qt5Widgets_FOUND AND Qt5Webkit_FOUND AND Qt5Network_FOUND)
	message(STATUS "QT5 webkit not found, skip QT5 view")
endif (Qt5Widgets_FOUND AND Qt5Webkit_FOUND AND Qt5Network_FOUND)

######################################################
add_subdirectory(lib)
add_subdirectory(integration)
add_subdirectory(trace-reader)
add_subdirectory(manpages)
if (QT5_FOUND)
	add_subdirectory(qtview)
endif (QT5_FOUND)

######################################################
# Install the webview into share directory
INSTALL(DIRECTORY webview DESTINATION share/malt
	PATTERN webview/client_v2/node_modules EXCLUDE)
