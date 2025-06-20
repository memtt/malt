############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 01/2016
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/valprof/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat - 2014 - 2016
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
include_directories(../)

############################################################
set(TEST_NAMES TestProfiledStateValue  
               TestProfiledCumulValue
               TestProfiledValue
               TestScatter2DValues)
               
############################################################
add_definitions(-DTEST_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/datas")

############################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base)
	malt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)

