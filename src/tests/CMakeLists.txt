######################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})

######################################################
# set(TEST_NAMES basic)

######################################################
# FOREACH(test_name ${TEST_NAMES})
# 	add_executable(${test_name} ${test_name}.cpp)
# 	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES})
# 	add_test(${test_name} ${test_name})
# ENDFOREACH(test_name)

add_executable(bench benchmark.cpp)
target_link_libraries(bench callstacktracebase htopml-json dl)