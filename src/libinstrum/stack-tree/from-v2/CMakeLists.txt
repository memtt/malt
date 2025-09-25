############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 10/2015
#    LICENSE  : CeCILL-C
#    FILE     : src/libinstrum/stack-tree/from-v2/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2015
############################################################

############################################################
set(MALT_STACKS_SRC Stack.cpp
                    BacktraceStack.cpp
                    EnterExitStack.cpp
                    RLockFreeTree.cpp
                    StackTreeMap.cpp)

############################################################
add_library(matt-stacks-v2 OBJECT ${MALT_STACKS_SRC})
SET_TARGET_PROPERTIES(matt-stacks-v2 PROPERTIES COMPILE_FLAGS -fPIC)

############################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)
