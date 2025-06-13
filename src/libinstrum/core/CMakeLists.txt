############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.0
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/core/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat - 2014
############################################################

############################################################
set(MALT_LIB_SRC Trigger.cpp
                 VmaTracker.cpp
                 SymbolSolver.cpp
                 CallStackInfo.cpp
                 ThreadTracker.cpp
                 AllocTraceFile.cpp
                 SegmentTracker.cpp
				 DomainCounters.cpp
                 ValgrindOutput.cpp
                 StackSizeTracker.cpp
                 StackSizeAnalyser.cpp
                 SimpleStackTracker.cpp
                 SimpleCallStackNode.cpp
                 PythonSymbolTracker.cpp)

############################################################
add_library(malt-core OBJECT ${MALT_LIB_SRC})
set_target_properties(malt-core PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)