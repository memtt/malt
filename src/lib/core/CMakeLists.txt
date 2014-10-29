######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MATT_LIB_SRC AllocTraceFile.cpp
                 CallStackInfo.cpp 
                 SymbolSolver.cpp 
                 SegmentTracker.cpp 
                 ValgrindOutput.cpp 
                 StackSizeTracker.cpp
                 SimpleStackTracker.cpp 
                 StackSizeAnalyser.cpp
                 AllocStackProfiler.cpp
                 SimpleCallStackNode.cpp
                 LocalAllocStackProfiler.cpp)

######################################################
add_library(matt-core OBJECT ${MATT_LIB_SRC})
SET_TARGET_PROPERTIES(matt-core PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)