############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.6
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/common/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat - 2014 - 2015
############################################################

############################################################
add_library(malt-common OBJECT CodeTiming.cpp FormattedMessage.cpp Debug.cpp Options.cpp SimpleAllocator.cpp Helpers.cpp NoFreeAllocator.cpp)
SET_TARGET_PROPERTIES(malt-common PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)