############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.0 
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/common/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat - 2014 - 2015
############################################################

############################################################
add_definitions(-DBUILD_PATH="${CMAKE_BINARY_DIR}")
add_definitions(-DSRC_PATH="${CMAKE_SOURCE_DIR}")

############################################################
add_library(malt-common OBJECT CodeTiming.cpp FormattedMessage.cpp Debug.cpp Options.cpp SimpleAllocator.cpp Helpers.cpp NoFreeAllocator.cpp StringIdDictionnary.cpp)
set_target_properties(malt-common PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)