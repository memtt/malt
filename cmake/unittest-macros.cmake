######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(ENABLE_VALGRIND     "no" CACHE BOOL "Enable automatic usage of valgrind memcheck on each unit test execution.")
set(ENABLE_JUNIT_OUTPUT "no" CACHE BOOL "Generate unit test output in junit format to be used by Jenkins or similar.")

######################################################
# Define some macro for unit test execution and definition
# It's better to use this wrapper to get good integration
# in jenkins infrastructure.

######################################################
#For integration of tests in jenkins, but only in self-test mode
macro (malt_add_test test_name)
	#steup wrapper
	if (ENABLE_VALGRIND)
			set(tmp_test_wrapper valgrind --xml=yes --xml-file=${CMAKE_CURRENT_BINARY_DIR}/${test_name}.memcheck.xml --tool=memcheck --leak-check=full --show-reachable=yes)
	else(ENABLE_VALGRIND)
			set(tmp_test_wrapper "")
	endif(ENABLE_VALGRIND)

	#setup output options
	if (ENABLE_JUNIT_OUTPUT)
		set(tmp_test_run_option --gtest_output=xml:${CMAKE_CURRENT_BINARY_DIR}/${test_name}.gtest.xml)
	endif (ENABLE_JUNIT_OUTPUT)

	#gen text command and register
	add_test(${test_name} ${tmp_test_wrapper} ${CMAKE_CURRENT_BINARY_DIR}/${test_name} ${tmp_test_run_option})
endmacro (malt_add_test)
