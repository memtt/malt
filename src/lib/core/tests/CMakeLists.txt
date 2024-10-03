############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/core/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014 - 2022
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
include_directories(../)

#####################################################
set(TEST_NAMES TestVmaTracker 
               TestSegmentTracker
               TestSymbolSolver
               TestAllocTraceFile
               TestCallStackInfo
)

#####################################################
add_library(TestSymbolSolverLib SHARED TestSymbolSolverLib.cpp)
target_link_libraries(TestSymbolSolverLib malt-base)

#####################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS "-g -O0")
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base TestSymbolSolverLib)
	malt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)
