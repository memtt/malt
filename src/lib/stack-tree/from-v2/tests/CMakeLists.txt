############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/stack-tree/from-v2/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2015
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
include_directories(../)

############################################################
#Enable openMP if present to use paralle tests
if(OPENMP_FOUND)
	message(STATUS "OpenMP found for unit tests")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -g")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -g")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS} -g")
endif()

############################################################
set(TEST_NAMES TestStack
               TestBacktraceStack
               TestEnterExitStack
               TestRLockFreeTree
               TestStackTreeMap)

############################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name}-v2 ${test_name}.cpp)
	target_link_libraries(${test_name}-v2 ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES}  malt-base)
	malt_add_test(${test_name}-v2 ${test_name}-v2)
ENDFOREACH(test_name)

