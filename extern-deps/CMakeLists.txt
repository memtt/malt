############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 10/2025
#    LICENSE  : CeCILL-C
#    FILE     : extern-deps/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat (INRIA) - 2024 - 2025
############################################################

############################################################
add_subdirectory(from-htopml)
add_subdirectory(renenyffenegger-base64)
# only if not found on system
if (NOT INIPARSER_FOUND)
	add_subdirectory(iniparser)
endif()
# only if enabled
if (ENABLE_JEMALLOC)
	add_subdirectory(jemalloc)
endif()
# only required for testing
if(ENABLE_TESTS AND GTEST_USE_EMBEDED)
	add_subdirectory(googletest-1.14.0)
endif()