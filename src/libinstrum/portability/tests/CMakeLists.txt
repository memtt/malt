############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 2.3.0
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/portability/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2019 - 2022
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
include_directories(../)

#####################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

#####################################################
add_definitions(-DCURRENT_SOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

#####################################################
set(TEST_NAMES TestOS TestCompiler TestProcMapReader TestMutex)

#####################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp MangledFunctions.cpp)
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base)
	malt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)
