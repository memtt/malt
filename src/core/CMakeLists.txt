######################################################
set(ATT_LIB_SRC FuncNameDic.cpp BacktraceCallStack.cpp SimpleStackTracer.cpp AllocStackProfiler.cpp CallStackInfo.cpp SimpleCallStackNode.cpp EnterExitCallStack.cpp Stack.cpp SegmentTracker.cpp ValgrindOutput.cpp)

######################################################
add_library(att-core OBJECT ${ATT_LIB_SRC})
SET_TARGET_PROPERTIES(att-core PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(att-libc-wrapper OBJECT AllocWrapper.cpp)
SET_TARGET_PROPERTIES(att-libc-wrapper PROPERTIES COMPILE_FLAGS -fPIC)