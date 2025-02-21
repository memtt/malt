############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/wrapper/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2022
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
include_directories(../)

############################################################
add_definitions(-DBINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DBUILD_DIR="${CMAKE_BINARY_DIR}")
add_definitions(-DSRC_DIR="${CMAKE_SOURCE_DIR}")

############################################################
set(TEST_NAMES
	TestThreadTracker
	TestInstrum
)

############################################################
if (PYTHON3_INSTRUM_FOUND)
	list(APPEND TEST_NAMES TestInstrumPython)
endif (PYTHON3_INSTRUM_FOUND)

############################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base)
	malt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)

############################################################
target_link_libraries(TestInstrum malt-injectors)
target_compile_options(TestInstrum PUBLIC -g -O0)
target_link_libraries(TestInstrum ${PYTHON3_LIBRARIES})

############################################################
if (PYTHON3_INSTRUM_FOUND)
	target_link_libraries(TestInstrumPython malt-injector-python)
	target_compile_options(TestInstrumPython PUBLIC -g)
	target_link_libraries(TestInstrumPython ${PYTHON3_LIBRARIES})
endif (PYTHON3_INSTRUM_FOUND)
