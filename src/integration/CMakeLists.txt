############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
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
# build MALT python interpreter
if (Python3_FOUND AND PYTHON3_INSTRUM_FOUND)
	include_directories(${Python3_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/src/libinstrum)
	add_executable(malt-python3 malt-python3.cpp)
	target_link_libraries(malt-python3 ${PYTHON3_LIBRARIES})
	target_link_libraries(malt-python3 malt)
	INSTALL(TARGETS malt-python3 DESTINATION bin/)
endif()

############################################################
#install
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/malt-webview ${CMAKE_CURRENT_BINARY_DIR}/malt-passwd
        DESTINATION bin/)

############################################################
if (ENABLE_PROFILER)
	INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/malt DESTINATION bin/)
	INSTALL(FILES MaltMPIRank.cpp DESTINATION share/malt)
endif (ENABLE_PROFILER)