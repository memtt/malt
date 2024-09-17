############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.2
#    DATE     : 06/2023
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/stack-tree/from-v2/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2015
############################################################
######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 06/2023                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MALT_STACKS_SRC Stack.cpp
                    BacktraceStack.cpp
                    EnterExitStack.cpp
                    RLockFreeTree.cpp
                    StackTreeMap.cpp)

######################################################
add_library(matt-stacks-v2 OBJECT ${MALT_STACKS_SRC})
SET_TARGET_PROPERTIES(matt-stacks-v2 PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)
