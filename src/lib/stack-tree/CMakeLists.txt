######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 1.3.0-dev                    #
#            DATE     : 03/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
set(MALT_STACK_TREES_SRC StackSTLHashMap.cpp)

######################################################
add_library(malt-stack-trees OBJECT ${MALT_STACK_TREES_SRC})
SET_TARGET_PROPERTIES(malt-stack-trees PROPERTIES COMPILE_FLAGS -fPIC)

######################################################
if (ENABLE_TESTS)
	add_subdirectory(tests)
endif (ENABLE_TESTS)

######################################################
add_subdirectory(from-v2)