############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 10/2025
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/stack-tree/from-v2/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2015
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
set(MALT_STACKS_SRC Stack.cpp
                    BacktraceStack.cpp
                    EnterExitStack.cpp
                    RLockFreeTree.cpp
                    StackTreeMap.cpp)

############################################################
add_library(matt-stacks-v2 OBJECT ${MALT_STACKS_SRC})
SET_TARGET_PROPERTIES(matt-stacks-v2 PROPERTIES COMPILE_FLAGS ${MALT_LIB_COMPILER_FLAGS})

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)
