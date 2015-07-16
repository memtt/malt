######################################################
#            PROJECT  : MALT                         #
#            VERSION  : 0.3.0                        #
#            DATE     : 07/2015                      #
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