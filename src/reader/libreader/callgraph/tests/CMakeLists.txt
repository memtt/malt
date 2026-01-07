############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 10/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/reader/libreader/callgraph/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

############################################################
add_definitions(-DBUILD_DIR="${CMAKE_BINARY_DIR}")
add_definitions(-DCUR_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DCUR_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions(-DSRC_DIR="${CMAKE_SOURCE_DIR}")

############################################################
add_executable(TestReaderCallGraph
    TestSimpleIdCache.cpp
    TestFuncMetrics.cpp
    TestCppDeclParser.cpp
    TestCallTreeAdapter.cpp
    TestDotCode.cpp
    TestGraphGenerator.cpp
)
target_link_libraries(TestReaderCallGraph malt-reader)
target_link_libraries(TestReaderCallGraph ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})
target_link_libraries(TestReaderCallGraph OpenMP::OpenMP_CXX)
add_test(TestReaderCallGraph TestReaderCallGraph)

