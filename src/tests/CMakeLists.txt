######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

######################################################
set(TEST_NAMES TestSegmentTracker TestProfiledValue TestFormattedMessage TestFastBufferedStream TestArray TestStack TestSimpleStackTracker)

######################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} matt-base)
	matt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)

add_executable(bench benchmark.cpp)
target_link_libraries(bench matt-base dl)

######################################################
#Enable openMP if present to use paralle tests
FIND_PACKAGE(OpenMP QUIET)
if(OPENMP_FOUND)
	message("OPENMP FOUND")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

######################################################
add_executable(simple-case-no-finstr simple-case.cpp)

######################################################
add_executable(simple-case-finstr simple-case.cpp)
SET_TARGET_PROPERTIES(simple-case-finstr PROPERTIES COMPILE_FLAGS -finstrument-functions)

######################################################
add_executable(simple-case-finstr-linked simple-case.cpp)
SET_TARGET_PROPERTIES(simple-case-finstr-linked PROPERTIES COMPILE_FLAGS -finstrument-functions)
target_link_libraries(simple-case-finstr-linked matt)
