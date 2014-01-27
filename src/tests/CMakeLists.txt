######################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

######################################################
set(TEST_NAMES TestSegmentTracker TestProfiledValue)

######################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} cstbase)
	add_test(${test_name} ${test_name})
ENDFOREACH(test_name)

add_executable(bench benchmark.cpp)
target_link_libraries(bench cstbase dl)

######################################################
add_executable(simple-case-no-finstr simple-case.cpp)

######################################################
add_executable(simple-case-finstr simple-case.cpp)
SET_TARGET_PROPERTIES(simple-case-finstr PROPERTIES COMPILE_FLAGS -finstrument-functions)

