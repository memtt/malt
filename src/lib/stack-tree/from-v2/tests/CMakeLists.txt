######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.0.0                        #
#            DATE     : 02/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
include_directories(../)

######################################################
set(TEST_NAMES TestStack
               TestBacktraceStack
               TestEnterExitStack
               TestRLockFreeTree
               TestStackTreeMap)

######################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name}-v2 ${test_name}.cpp)
	target_link_libraries(${test_name}-v2 ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES}  malt-base)
	malt_add_test(${test_name}-v2 ${test_name}-v2)
ENDFOREACH(test_name)

