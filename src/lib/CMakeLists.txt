######################################################
#            PROJECT  : htopml                       #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 11/2012                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(CST_SOURCES FuncNameDic.cpp SimpleCallStack.cpp SimpleStackTracer.cpp AllocStackProfiler.cpp CallStackInfo.cpp SimpleCallStackNode.cpp)

######################################################
add_library(callstacktracebase STATIC ${CST_SOURCES})
SET_TARGET_PROPERTIES(callstacktracebase PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(callstacktrace STATIC ${CST_SOURCES} AllocWrapper.cpp)
SET_TARGET_PROPERTIES(callstacktrace PROPERTIES COMPILE_FLAGS -fPIC)