############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/integration/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2019 - 2022
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
#Generate file with full paths
if (ENABLE_PROFILER)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/malt.sh.in ${CMAKE_CURRENT_BINARY_DIR}/malt @ONLY)
	install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/malt DESTINATION bin/)
endif (ENABLE_PROFILER)
if (ENABLE_PROFILER AND PYTHON3_INSTRUM_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/malt-python.sh.in ${CMAKE_CURRENT_BINARY_DIR}/malt-python @ONLY)
	install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/malt-python DESTINATION bin/)
endif ()

############################################################
#Generate file with full paths
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/malt-webview.sh.in ${CMAKE_CURRENT_BINARY_DIR}/malt-webview @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/malt-passwd.sh.in ${CMAKE_CURRENT_BINARY_DIR}/malt-passwd @ONLY)

############################################################
#install
if (NODE_FOUND)
	install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/malt-webview ${CMAKE_CURRENT_BINARY_DIR}/malt-passwd
			DESTINATION bin/)
endif (NODE_FOUND)

############################################################
if (ENABLE_PROFILER)
	install(FILES MaltMPIRank.cpp DESTINATION share/malt)
endif (ENABLE_PROFILER)