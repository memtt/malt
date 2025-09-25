############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 07/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/reader/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
add_subdirectory(libreader)
if (WEBVIEW_SERVER STREQUAL "RUST")
    add_subdirectory(webview-2-rust)
endif ()
if (WEBVIEW_SERVER STREQUAL "CPP")
	add_subdirectory(webview-2-cpp)
endif()
