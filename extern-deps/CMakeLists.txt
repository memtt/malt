############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 06/2025
#    LICENSE  : CeCILL-C
#    FILE     : extern-deps/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat (INRIA) - 2024 - 2025
############################################################

############################################################
add_subdirectory(from-htopml)
add_subdirectory(iniparser)
add_subdirectory(renenyffenegger-base64)
if (ENABLE_JEMALLOC)
	add_subdirectory(jemalloc)
endif()
# only required for testing
if(ENABLE_TESTS AND GTEST_USE_EMBEDED)
	add_subdirectory(googletest-1.14.0)
endif()