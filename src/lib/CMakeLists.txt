######################################################
set(CST_SOURCES FuncNameDic.cpp BacktraceCallStack.cpp SimpleStackTracer.cpp AllocStackProfiler.cpp CallStackInfo.cpp SimpleCallStackNode.cpp EnterExitCallStack.cpp Stack.cpp)

######################################################
add_library(libcst OBJECT ${CST_SOURCES})
SET_TARGET_PROPERTIES(libcst PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(allocwrapper OBJECT AllocWrapper.cpp)
SET_TARGET_PROPERTIES(allocwrapper PROPERTIES COMPILE_FLAGS -fPIC)