############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.2
#    DATE     : 06/2023
#    LICENSE  : CeCILL-C
#    FILE     : src/lib/stacks/CMakeLists.txt
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2014
#    AUTHOR   : Sébastien Valat (ECR) - 2014 - 2015
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
                    StackLoopRemover.cpp)

######################################################
add_library(malt-stacks OBJECT ${MALT_STACKS_SRC})
SET_TARGET_PROPERTIES(malt-stacks PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)