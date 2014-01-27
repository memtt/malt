######################################################
set(ATT_LIB_SRC Stack.cpp 
                FuncNameDic.cpp 
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
add_library(att-core OBJECT ${ATT_LIB_SRC})
SET_TARGET_PROPERTIES(att-core PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
add_library(att-libc-wrapper OBJECT AllocWrapper.cpp)
SET_TARGET_PROPERTIES(att-libc-wrapper PROPERTIES COMPILE_FLAGS -fPIC)