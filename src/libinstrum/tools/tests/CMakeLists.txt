############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.6
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/tools/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

############################################################
add_definitions(-DTEST_BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}")

############################################################
set(TEST_NAMES TestProcPagemapReader TestNMCmdReader TestAddr2Line)

############################################################
if (LIBELF_FOUND)
	list(APPEND TEST_NAMES TestElfReader)
endif()

############################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

############################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base)
	malt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)

############################################################
if (LIBELF_FOUND)
	add_library(TestElfReaderDyn SHARED TestElfReaderDyn.cpp)
	target_link_libraries(TestElfReader TestElfReaderDyn)
endif()

############################################################
target_compile_options(TestAddr2Line PRIVATE -O0 -g)
