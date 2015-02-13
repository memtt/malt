######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.0.0                        #
#            DATE     : 02/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
add_library(matt-core OBJECT CallCounter.cpp Clock.cpp Options.cpp UserSegmentTracker.cpp PeakTracker.cpp TimeProfiler.cpp)
SET_TARGET_PROPERTIES(matt-core PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)