######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC OFF)

######################################################

#includes
include_directories(${Qt5Widgets_INCLUDE_DIRS})
if (Qt5WebEngineWidgets_FOUND)
	include_directories(${Qt5WebEngineWidgets_INCLUDE_DIRS})
	add_definitions(-DHAVE_QT5_WEB_ENGINE_WIDGETS)
endif(Qt5WebEngineWidgets_FOUND)
if (Qt5WebKitWidgets_FOUND)
	include_directories(${Qt5WebKitWidgets_INCLUDE_DIRS})
	add_definitions(-DHAVE_QT5_WEBKIT_WIDGETS)
endif(Qt5WebKitWidgets_FOUND)
include_directories(${Qt5Network_INCLUDE_DIRS})

# path to prefix
add_definitions(-DMALT_PREFIX="${CMAKE_INSTALL_PREFIX}")

# Tell CMake to create the helloworld executable
add_executable(malt-qt5 main.cpp)

# Use the Widgets module from Qt 5.
target_link_libraries(malt-qt5 Qt5::Widgets Qt5::Network)
if (Qt5WebEngineWidgets_FOUND)
	target_link_libraries(malt-qt5 Qt5::WebEngineWidgets)
endif (Qt5WebEngineWidgets_FOUND)
if (Qt5WebKitWidgets_FOUND)
	target_link_libraries(malt-qt5 Qt5::WebKitWidgets)
endif (Qt5WebKitWidgets_FOUND)

#install
install(TARGETS malt-qt5 DESTINATION bin)
