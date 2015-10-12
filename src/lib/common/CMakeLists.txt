######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.3.0                        #
#            DATE     : 07/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_library(malt-common OBJECT CodeTiming.cpp FormattedMessage.cpp Debug.cpp Options.cpp SimpleAllocator.cpp Helpers.cpp NoFreeAllocator.cpp)
SET_TARGET_PROPERTIES(malt-common PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)