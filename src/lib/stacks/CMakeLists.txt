######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.1.0                        #
#            DATE     : 03/2015                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MALT_STACKS_SRC Stack.cpp
                    BacktraceStack.cpp
                    EnterExitStack.cpp)

######################################################
add_library(malt-stacks OBJECT ${MALT_STACKS_SRC})
SET_TARGET_PROPERTIES(malt-stacks PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)