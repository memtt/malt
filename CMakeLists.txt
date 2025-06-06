############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.6
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat - 2015 - 2022
############################################################

############################################################
add_subdirectory(libreader)
if (CARGO_FOUND)
    add_subdirectory(webview-2-rust)
endif ()
if (HTTPLIB_FOUND AND CRYPTO_FOUND)
	add_subdirectory(webview-2-cpp)
endif()
