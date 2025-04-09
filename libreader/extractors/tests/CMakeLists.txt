############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014 - 2020
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
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
add_executable(TestReaderExtractors
    TestExtractorHelpers.cpp
    TestExtractor.cpp
    TestSimpleIdCache.cpp
    TestFuncMetrics.cpp
    TestCppDeclParser.cpp
)
target_link_libraries(TestReaderExtractors malt-profile)
target_link_libraries(TestReaderExtractors ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})
target_link_libraries(TestReaderExtractors OpenMP::OpenMP_CXX)
add_test(TestReaderExtractors TestReaderExtractors)

############################################################
add_executable(extractor-example example.cpp)
target_link_options(extractor-example PRIVATE -O0 -g -finstrument-functions)
target_link_libraries(extractor-example PRIVATE malt)
