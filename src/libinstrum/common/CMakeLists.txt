############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 01/2026
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/common/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014
#    AUTHOR   : Sébastien Valat - 2014 - 2026
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
add_definitions(-DBUILD_PATH="${CMAKE_BINARY_DIR}")
add_definitions(-DSRC_PATH="${CMAKE_SOURCE_DIR}")

############################################################
add_library(malt-common OBJECT
	CodeTiming.cpp
	FormattedMessage.cpp
	Debug.cpp
	OptionsMeta.cpp
	Options.cpp
	SimpleAllocator.cpp
	Helpers.cpp
	NoFreeAllocator.cpp
	StringIdDictionnary.cpp
)
set_target_properties(malt-common PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)