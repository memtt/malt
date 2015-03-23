######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MALT_LIB_SRC VmaTracker.cpp
                 SymbolSolver.cpp
                 CallStackInfo.cpp
                 AllocTraceFile.cpp
                 SegmentTracker.cpp
                 ValgrindOutput.cpp
                 StackSizeTracker.cpp
                 StackSizeAnalyser.cpp
                 SimpleStackTracker.cpp
                 SimpleCallStackNode.cpp)

######################################################
add_library(malt-core OBJECT ${MALT_LIB_SRC})
SET_TARGET_PROPERTIES(malt-core PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)