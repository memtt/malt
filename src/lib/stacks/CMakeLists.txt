############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.5
#    DATE     : 02/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/stacks/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
############################################################

############################################################
set(MALT_STACKS_SRC Stack.cpp
                    BacktraceStack.cpp
                    EnterExitStack.cpp
                    StackLoopRemover.cpp)

############################################################
add_library(malt-stacks OBJECT ${MALT_STACKS_SRC})
SET_TARGET_PROPERTIES(malt-stacks PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)