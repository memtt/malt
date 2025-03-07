############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat - 2014 - 2018
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

############################################################
add_definitions(-DTEST_BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}")

############################################################
set(TEST_NAMES TestFastBufferedStream 
               TestSimpleStackTracker 
               TestValgrindOutput
   )

############################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base)
	malt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)

add_executable(bench benchmark.cpp)
target_link_libraries(bench malt-base dl)

############################################################
#Enable openMP if present to use paralle tests
if(OPENMP_FOUND)
	message(STATUS "OpenMP found for unit tests")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

############################################################
set(CMAKE_CXX_FLAGS -O0)

############################################################
add_executable(simple-case-no-finstr simple-case.cpp)
target_compile_options(simple-case-no-finstr PRIVATE -g -O0)

############################################################
add_executable(simple-case-finstr simple-case.cpp)
target_compile_options(simple-case-finstr PRIVATE -g -O0 -finstrument-functions)

############################################################
add_executable(simple-case-finstr-linked simple-case.cpp)
target_link_libraries(simple-case-finstr-linked malt)
target_compile_options(simple-case-finstr-linked PRIVATE -g -O0 -finstrument-functions)

############################################################
add_executable(test-atexit-first test-atexit-first.cpp)
target_link_libraries(test-atexit-first malt)
target_compile_options(test-atexit-first PRIVATE -g -O0 -finstrument-functions)

############################################################
add_executable(test-fragmentation test-fragmentation.cpp)
target_link_libraries(test-fragmentation malt)
target_compile_options(test-fragmentation PRIVATE -g -O0 -finstrument-functions)

############################################################
add_executable(test-controler test-controler.cpp)
target_link_libraries(test-controler malt-controler)
target_compile_options(test-controler PRIVATE -g -O0 -finstrument-functions)

############################################################
add_executable(test-pthread-tracking test-pthread-tracking.cpp)
target_link_libraries(test-pthread-tracking malt)

############################################################
add_definitions(-DBUILD_PATH="${CMAKE_BINARY_DIR}")
add_definitions(-DEXE_PATH="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DSRC_PATH="${CMAKE_SOURCE_DIR}")

############################################################
add_executable(TestFullAnalysis TestFullAnalysis.cpp)
target_link_libraries(TestFullAnalysis ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base)
add_test(TestFullAnalysis TestFullAnalysis)
