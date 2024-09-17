############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.2
#    DATE     : 06/2023
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/core/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
#    AUTHOR   : Sébastien Valat - 2014
############################################################
######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 06/2023                      #
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