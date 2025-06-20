############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/common/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat - 2014 - 2022
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
include_directories(../)

############################################################
add_definitions(-DTEST_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions(-DTEST_BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}")

############################################################
set(TEST_NAMES TestArray
               TestOptions
               TestSimpleAllocator
               TestSTLInternalAllocator
               TestFormattedMessage
               TestCodeTiming
               TestDebug
               TestNoFreeAllocator
               TestHelpers
               TestParallelWorker
               TestRangePtr
               TestStringIdDictionnary
   )

############################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base)
	malt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)