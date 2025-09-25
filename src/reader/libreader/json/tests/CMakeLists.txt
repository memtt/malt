############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 03/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/reader/libreader/json/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################
add_executable(TestJson TestParser.cpp TestJson.cpp)
target_link_libraries(TestJson ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-profile)