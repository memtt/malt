######################################################
#            PROJECT  : htopml                       #
#            VERSION  : 0.1.0-dev                    #
#            DATE     : 11/2012                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(CST_SOURCES FuncNameDic.cpp SimpleCallStack.cpp SimpleStackTracer.cpp AllocWrapper.cpp AllocStackProfiler.cpp CallStackInfo.cpp)

######################################################
add_library(callstacktrace STATIC ${CST_SOURCES})
SET_TARGET_PROPERTIES(callstacktrace PROPERTIES COMPILE_FLAGS -fPIC)
