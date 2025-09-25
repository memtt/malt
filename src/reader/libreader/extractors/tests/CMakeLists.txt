############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/reader/libreader/extractors/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src/libinstrum)

############################################################
add_definitions(-DBUILD_DIR="${CMAKE_BINARY_DIR}")
add_definitions(-DCUR_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DCUR_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions(-DSRC_DIR="${CMAKE_SOURCE_DIR}")

############################################################
add_executable(TestReaderExtractors
    TestExtractorHelpers.cpp
    TestExtractor.cpp
)
target_link_libraries(TestReaderExtractors malt-profile)
target_link_libraries(TestReaderExtractors ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})
target_link_libraries(TestReaderExtractors OpenMP::OpenMP_CXX)
add_test(TestReaderExtractors TestReaderExtractors)

############################################################
add_executable(extractor-example example.cpp)
target_compile_options(extractor-example PUBLIC -O0 -g -finstrument-functions)
target_link_libraries(extractor-example PUBLIC malt)
