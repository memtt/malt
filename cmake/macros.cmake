############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.5
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : cmake/macros.cmake
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014 - 2024
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat (INRIA) - 2024
############################################################

############################################################
#Setup paths to gtest/gmock headers and library
MACRO(malt_setup_internal_gmock_and_gtest)
	set(GMOCK_SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern-deps/googletest-1.14.0)
	set(GMOCK_INCLUDE_DIR ${GMOCK_SOURCE_DIR}/googlemock/include)
	set(GMOCK_INCLUDE_DIRS ${GMOCK_SOURCE_DIR}/googlemock/include)
	set(GMOCK_BOTH_LIBRARIES gmock gmock_main)
	set(GTEST_BOTH_LIBRARIES gtest)
	set(GTEST_INCLUDE_DIR ${GMOCK_SOURCE_DIR}/googletest//include/)
	set(GTEST_INCLUDE_DIRS ${GMOCK_SOURCE_DIR}/googletest//include/)
	set(GTEST_USE_EMBEDED "yes")
ENDMACRO(malt_setup_internal_gmock_and_gtest)

############################################################
# Setup google test by either using the internal one of using the one is current system if avail
MACRO(malt_setup_google_tests)
	# if avail use the system one, otherwise use embeded one
	if (NOT GTEST_FOUND)
		malt_setup_internal_gmock_and_gtest()
	else()
		set(GTEST_USE_EMBEDED "no")
	endif()
ENDMACRO()

############################################################
#Short macro to quicly declare some unit tests
MACRO(declare_test test_name)
	add_executable(${test_name} ${test_name}.cpp)
	target_link_libraries(${test_name} ${GTEST_BOTH_LIBRARIES} ${GMOCK_BOTH_LIBRARIES} ${EXTRA_LIBS})
	add_test(${test_name} ${test_name})
ENDMACRO(declare_test)

############################################################
MACRO(malt_enable_gcc_coverage)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fprofile-arcs -ftest-coverage -fprofile-update=atomic")
	set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -O0 -fprofile-arcs -ftest-coverage -fprofile-update=atomic")
	set(CMAKE_EXE_LINKER_FLAGS_FLAGS "${CMAKE_C_FLAGS} -O0 -fprofile-arcs -ftest-coverage -fprofile-update=atomic")
ENDMACRO(malt_enable_gcc_coverage)

############################################################
MACRO(check_nodejs)
	find_program(NODEJS_PATH NAMES node nodejs)
	if (NOT NODEJS_PATH)
		message(WARNING "Failed to find nodeJS program, the webview GUI will not work !")
	endif(NOT NODEJS_PATH)
ENDMACRO(check_nodejs)

############################################################
MACRO(malt_enable_cxx_11)
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	if(COMPILER_SUPPORTS_CXX11)
		message(STATUS "Using -std=c++11")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	else()
			message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
	endif()
ENDMACRO(malt_enable_cxx_11)

###########################################################
# Print a summary status to help ensuring everything
# is correct
function(malt_print_status)
	# Prepare some vars for printing
	list(JOIN MALT_CXX_FLAGS " " MALT_CXX_FLAGS_STR)
	string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_UPPER)
	if (CMAKE_BUILD_TYPE STREQUAL "")
		list(JOIN CMAKE_CXX_FLAGS " " CMAKE_BUILD_TYPE_FLAGS)
	else()
		list(JOIN CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER} " " CMAKE_BUILD_TYPE_FLAGS)
	endif()

	# Print summary
	message(STATUS "==============================================================")
	message(STATUS "|  OS               : ${CMAKE_HOST_SYSTEM_NAME}")
	message(STATUS "|  Compiler familly : ${CMAKE_CXX_COMPILER_ID}")
	message(STATUS "|  Compiler         : ${CMAKE_CXX_COMPILER}")
	message(STATUS "--------------------------------------------------------------")
	message(STATUS "|  tests            : ${ENABLE_TESTS}")
	message(STATUS "|  profiler         : ${ENABLE_PROFILER}")
	message(STATUS "|  gcc-coverage     : ${ENABLE_GCC_COVERAGE}")
	message(STATUS "|  valgrind         : ${ENABLE_VALGRIND}")
	message(STATUS "--------------------------------------------------------------")
	message(STATUS "|  openmp           : ${OpenMP_CXX_FLAGS}")
	message(STATUS "|  libunwind        : ${LIBUNWIND_LIBRARIES}")
	message(STATUS "|  libelf           : ${LIBELF_LIBRARY}")
	#message(STATUS "|  iniparser        : ${INIPARSER_LIBRARY}")
	message(STATUS "|  gtest            : ${GTEST_INCLUDE_DIR}")
	message(STATUS "|  qt5-widgets      : ${Qt5Widgets_INCLUDE_DIRS}")
	message(STATUS "|  qt5-web-egnine   : ${Qt5WebEngineWidgets_INCLUDE_DIRS}")
	message(STATUS "|  qt5-network      : ${Qt5Network_INCLUDE_DIRS}")
	message(STATUS "--------------------------------------------------------------")
	message(STATUS "|  CMake build type : ${CMAKE_BUILD_TYPE}")
	message(STATUS "|  CMake cxxflags   : ${CMAKE_BUILD_TYPE_FLAGS}")
	message(STATUS "|  User cxxflags    : ${CMAKE_CXX_FLAGS}")
	message(STATUS "|  All cxxflags     : ${CMAKE_BUILD_TYPE_FLAGS} ${MALT_CXX_FLAGS_STR} ${CMAKE_CXX_FLAGS}")
	message(STATUS "==============================================================")
endfunction()
