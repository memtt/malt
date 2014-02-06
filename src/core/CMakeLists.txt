######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 01/2014                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MATT_LIB_SRC Stack.cpp 
                 SymbolResolver.cpp 
                 CallStackInfo.cpp 
                 ProfiledValue.cpp
                 SegmentTracker.cpp 
                 ValgrindOutput.cpp 
                 SimpleStackTracer.cpp 
                 EnterExitCallStack.cpp 
                 BacktraceCallStack.cpp 
                 AllocStackProfiler.cpp 
                 SimpleCallStackNode.cpp)

######################################################
add_library(matt-core OBJECT ${MATT_LIB_SRC})
SET_TARGET_PROPERTIES(matt-core PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(matt-libc-wrapper OBJECT AllocWrapper.cpp)
SET_TARGET_PROPERTIES(matt-libc-wrapper PROPERTIES COMPILE_FLAGS -fPIC)