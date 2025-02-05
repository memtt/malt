############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.5
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/integration/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2014 - 2019
############################################################

############################################################
#Generate file with full paths
if (ENABLE_PROFILER)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/malt.sh.in ${CMAKE_CURRENT_BINARY_DIR}/malt @ONLY)
endif (ENABLE_PROFILER)

############################################################
#Generate file with full paths
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/malt-webview.sh.in ${CMAKE_CURRENT_BINARY_DIR}/malt-webview @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/malt-passwd.sh.in ${CMAKE_CURRENT_BINARY_DIR}/malt-passwd @ONLY)

############################################################
#install
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/malt-webview ${CMAKE_CURRENT_BINARY_DIR}/malt-passwd
        DESTINATION bin/)

############################################################
if (ENABLE_PROFILER)
	INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/malt DESTINATION bin/)
	INSTALL(FILES MaltMPIRank.cpp DESTINATION share/malt)
endif (ENABLE_PROFILER)