######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.0.0                        #
#            DATE     : 02/2018                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MALT_STACKS_SRC Stack.cpp
                    BacktraceStack.cpp
                    EnterExitStack.cpp
                    StackLoopRemover.cpp)

######################################################
add_library(malt-stacks OBJECT ${MALT_STACKS_SRC})
SET_TARGET_PROPERTIES(malt-stacks PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)