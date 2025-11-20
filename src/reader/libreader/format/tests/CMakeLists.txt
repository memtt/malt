############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 10/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/reader/libreader/format/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

############################################################
add_definitions(-DBUILD_DIR="${CMAKE_BINARY_DIR}")
add_definitions(-DCUR_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}")

############################################################
add_executable(TestReaderFormat
    TestFormat.cpp
    TestStacks.cpp
)
target_link_libraries(TestReaderFormat malt-reader)
target_link_libraries(TestReaderFormat ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})
add_test(TestReaderFormat TestReaderFormat)

