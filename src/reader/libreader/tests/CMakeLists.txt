############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 07/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/reader/libreader/tests/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
#Use gtest
include_directories(${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS})
include_directories(../)
include_directories(${CMAKE_BINARY_DIR}/src/libinstrum/)
if (PYTHON3_INSTRUM_FOUND)
	include_directories(${Python3_INCLUDE_DIRS})
endif()

############################################################
add_definitions(-DBINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DBUILD_DIR="${CMAKE_BINARY_DIR}")
add_definitions(-DSRC_DIR="${CMAKE_SOURCE_DIR}")
add_definitions(-DCUR_SRC_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions(-DEXE_PATH="${CMAKE_BINARY_DIR}/src/libinstrum/tests")

############################################################
set(TEST_NAMES TestInstrum)

############################################################
if (PYTHON3_INSTRUM_FOUND)
	list(APPEND TEST_NAMES TestInstrumPython)
endif ()

############################################################
FOREACH(test_name ${TEST_NAMES})
	add_executable(${test_name} ${test_name}.cpp)
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base)
	malt_add_test(${test_name} ${test_name})
ENDFOREACH(test_name)

############################################################
target_link_libraries(TestInstrum malt-injectors)
target_compile_options(TestInstrum PRIVATE -g -O0)
target_link_libraries(TestInstrum malt-profile)

############################################################
if (PYTHON3_INSTRUM_FOUND)
	target_compile_options(TestInstrumPython PRIVATE -g)
	target_link_libraries(TestInstrumPython ${Python3_LIBRARIES})
	target_link_libraries(TestInstrumPython malt-profile)
endif ()

############################################################
if (NLOHMANNJSON_FOUND)
	add_executable(TestFullAnalysis TestFullAnalysis.cpp)
	target_link_libraries(TestFullAnalysis ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} malt-base malt-profile)
	target_compile_options(TestFullAnalysis PRIVATE -g -O0)
	add_test(TestFullAnalysis TestFullAnalysis)
endif()
