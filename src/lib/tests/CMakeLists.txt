######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

######################################################
add_definitions(-DTEST_BIN_DIR="${CMAKE_CURRENT_BINARY_DIR}")

######################################################
set(TEST_NAMES TestFastBufferedStream 
               TestSimpleStackTracker 
               TestValgrindOutput
   )

######################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base)
	malt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)

add_executable(bench benchmark.cpp)
target_link_libraries(bench malt-base dl)

######################################################
#Enable openMP if present to use paralle tests
FIND_PACKAGE(OpenMP QUIET)
if(OPENMP_FOUND)
	message(STATUS "OpenMP found for unit tests")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS} -g")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -g")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS} -g")
endif()

######################################################
add_executable(simple-case-no-finstr simple-case.cpp)

######################################################
add_executable(simple-case-finstr simple-case.cpp)

######################################################
add_executable(simple-case-finstr-linked simple-case.cpp)
target_link_libraries(simple-case-finstr-linked malt)

######################################################
add_executable(test-atexit-first test-atexit-first.cpp)
SET_TARGET_PROPERTIES(test-atexit-first PROPERTIES COMPILE_FLAGS -finstrument-functions)
target_link_libraries(test-atexit-first malt)

######################################################
add_executable(test-fragmentation test-fragmentation.cpp)
SET_TARGET_PROPERTIES(test-fragmentation PROPERTIES COMPILE_FLAGS -finstrument-functions)
target_link_libraries(test-fragmentation malt)

######################################################
add_executable(test-controler test-controler.cpp)
SET_TARGET_PROPERTIES(test-controler PROPERTIES COMPILE_FLAGS -finstrument-functions)
target_link_libraries(test-controler malt-controler)

######################################################
add_executable(test-pthread-tracking test-pthread-tracking.cpp)
target_link_libraries(test-pthread-tracking malt)
