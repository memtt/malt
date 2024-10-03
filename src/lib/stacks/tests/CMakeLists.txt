############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/stacks/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014
#    AUTHOR   : Sébastien Valat (ECR) - 2014
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
include_directories(../)

############################################################
set(TEST_NAMES TestStack
               TestBacktraceStack
               TestEnterExitStack)

############################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base)
	malt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)

