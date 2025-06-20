############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.1
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/core/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014 - 2022
#    AUTHOR   : Sébastien Valat (ECR) - 2015
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
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
               TestSimpleCallStackNode
               TestSimpleCallStackTracker
               TestStackSizeAnalyser
)

#####################################################
add_library(TestSymbolSolverLib SHARED TestSymbolSolverLib.cpp)
target_link_libraries(TestSymbolSolverLib malt-base)
target_compile_options(TestSymbolSolverLib PRIVATE -g -O0)

#####################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	set_target_properties(${test_name} PROPERTIES COMPILE_FLAGS "-g -O0")
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base TestSymbolSolverLib)
	malt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)

#####################################################
target_compile_options(TestSymbolSolver PRIVATE -g)
