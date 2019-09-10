######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.0-dev                    #
#            DATE     : 08/2019                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
include_directories(../)

#####################################################
set(TEST_NAMES TestVmaTracker TestSegmentTracker TestSymbolSolver)

#####################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS "-g -O0")
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base)
	malt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)
