######################################################
#            PROJECT  : htopml                       #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 11/2012                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(CST_SOURCES FuncNameDic.cpp BacktraceCallStack.cpp SimpleStackTracer.cpp AllocStackProfiler.cpp CallStackInfo.cpp SimpleCallStackNode.cpp EnterExitCallStack.cpp Stack.cpp)

######################################################
add_library(libcst OBJECT ${CST_SOURCES})
SET_TARGET_PROPERTIES(libcst PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(allocwrapper OBJECT AllocWrapper.cpp)
SET_TARGET_PROPERTIES(allocwrapper PROPERTIES COMPILE_FLAGS -fPIC)